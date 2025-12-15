#include <cstdint>
#include <cstdio>
#include <unordered_set>
#include <thread>
#include <atomic>
#include <stdafx.h>
#include <cpu/ppc_context.h>
#include <cpu/guest_thread.h>
#include <apu/audio.h>
#include <gpu/video.h>
#include <ui/game_window.h>
#include "function.h"
#include "xex.h"
#include "xbox.h"
#include "heap.h"
#include "memory.h"
#include <memory>
#include "xam.h"
#include "xdm.h"
#include <user/config.h>
#include <user/paths.h>
#include <os/logger.h>

#include "io/file_system.h"

#include <cerrno>
#include <fstream>
#include <algorithm>
#include <vector>

#if defined(__APPLE__)
#include <CommonCrypto/CommonCryptor.h>
#endif

#ifdef _WIN32
#include <ntstatus.h>
#endif

// =============================================================================
// SDL Event Pumping Helper
// =============================================================================
// This is called from kernel functions that are invoked frequently during
// game execution to keep the window responsive. Without this, the window
// freezes because GuestThread::Start blocks the main thread.
// 
// IMPORTANT: On macOS, SDL event pumping MUST happen on the main thread.
// If called from a worker thread, we skip the pump to avoid crashes.

static std::chrono::steady_clock::time_point g_lastSdlPumpTime;
static constexpr auto SDL_PUMP_INTERVAL = std::chrono::milliseconds(16); // ~60fps
static std::thread::id g_kernelMainThreadId;
static std::atomic<bool> g_kernelMainThreadIdSet{false};

void InitKernelMainThread()
{
    g_kernelMainThreadId = std::this_thread::get_id();
    g_kernelMainThreadIdSet = true;
}

bool IsMainThread()
{
    if (!g_kernelMainThreadIdSet) return false;
    return std::this_thread::get_id() == g_kernelMainThreadId;
}

void PumpSdlEventsIfNeeded()
{
    // On macOS, SDL event pumping MUST happen on the main thread
    // Skip if we're on a worker thread to avoid Cocoa crashes
    if (!IsMainThread()) return;
    
    auto now = std::chrono::steady_clock::now();
    if (now - g_lastSdlPumpTime >= SDL_PUMP_INTERVAL)
    {
        g_lastSdlPumpTime = now;
        SDL_PumpEvents();
        
        // Process critical events (window close, etc.)
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                std::_Exit(0);
            }
        }
    }
}

// =============================================================================

std::unordered_map<uint32_t, uint32_t> g_handleDuplicates{};

namespace
{
    constexpr uint32_t STATUS_INVALID_HANDLE = 0xC0000008;
    constexpr uint32_t STATUS_INVALID_PARAMETER = 0xC000000D;
    constexpr uint32_t STATUS_INFO_LENGTH_MISMATCH = 0xC0000004;
    constexpr uint32_t STATUS_END_OF_FILE = 0xC0000011;
    constexpr uint32_t STATUS_ACCESS_DENIED = 0xC0000022;
    constexpr uint32_t STATUS_OBJECT_NAME_NOT_FOUND = 0xC0000034;
    constexpr uint32_t STATUS_OBJECT_PATH_NOT_FOUND = 0xC000003A;
    constexpr uint32_t STATUS_OBJECT_NAME_INVALID = 0xC0000033;
    constexpr uint32_t STATUS_OBJECT_NAME_COLLISION = 0xC0000035;
    constexpr uint32_t STATUS_BUFFER_OVERFLOW = 0x80000005;
    constexpr uint32_t STATUS_NO_MORE_FILES = 0x80000006;

    // FILE_INFORMATION_CLASS (subset)
    constexpr uint32_t FileBasicInformation = 4;
    constexpr uint32_t FileStandardInformation = 5;
    constexpr uint32_t FilePositionInformation = 14;
    constexpr uint32_t FileEndOfFileInformation = 20;
    constexpr uint32_t FileNetworkOpenInformation = 34;

    // FS_INFORMATION_CLASS (subset)
    constexpr uint32_t FileFsSizeInformation = 3;
    constexpr uint32_t FileFsDeviceInformation = 4;
    constexpr uint32_t FileFsAttributeInformation = 5;

    constexpr uint32_t kNtFileHandleMagic = 0x4E544649; // 'NTFI'
    constexpr uint32_t kNtDirHandleMagic = 0x4E544449; // 'NTDI'
    constexpr uint32_t kNtVirtFileHandleMagic = 0x4E545646; // 'NTVF'

    struct NtFileHandle final : KernelObject
    {
        uint32_t magic = kNtFileHandleMagic;
        std::fstream stream;
        std::filesystem::path path;
        // RPF metadata cached per-open
        bool rpfHeaderParsed = false;
        bool isRpf = false;
        uint32_t tocOffset = 0;
        uint32_t tocSize = 0;
        bool tocEncrypted = false;
    };

    struct NtDirHandle final : KernelObject
    {
        uint32_t magic = kNtDirHandleMagic;
        std::filesystem::path path;
        std::vector<std::filesystem::directory_entry> entries;
        size_t cursor = 0;
        bool initialized = false;
    };

    struct NtVirtFileHandle final : KernelObject
    {
        uint32_t magic = kNtVirtFileHandleMagic;
        std::filesystem::path debugPath;
        std::vector<uint8_t> data;
    };

    static std::unordered_map<uint32_t, NtFileHandle*> g_ntFileHandles;
    static std::unordered_map<uint32_t, NtDirHandle*> g_ntDirHandles;
    static std::unordered_map<uint32_t, NtVirtFileHandle*> g_ntVirtFileHandles;
    
    // GPU Ring Buffer state for fake GPU consumption
    struct GpuRingBufferState {
        uint32_t ringBufferBase = 0;      // Physical address of ring buffer
        uint32_t ringBufferSize = 0;      // Size in bytes (1 << size_log2)
        uint32_t readPtrWritebackAddr = 0; // Address where GPU writes read pointer
        uint32_t blockSize = 0;           // Block size for writeback (1 << block_size_log2)
        uint32_t interruptCallback = 0;   // Graphics interrupt callback function
        uint32_t interruptUserData = 0;   // User data for interrupt callback
        bool initialized = false;
        bool writebackEnabled = false;
        bool interruptFired = false;
        
        // Persistent video state flags - the GPU polling loop checks these
        bool enginesInitialized = false;
        bool edramTrainingComplete = false;
        bool interruptSeen = false;
    };
    static GpuRingBufferState g_gpuRingBuffer;
    
    // Global RPF streams for translation layer - opened once, used for all reads
    struct RpfStreamInfo {
        std::fstream stream;
        std::filesystem::path path;
        uint64_t fileSize = 0;
        bool headerParsed = false;
        uint32_t tocOffset = 0;
        uint32_t tocSize = 0;
        bool tocEncrypted = false;
        std::vector<uint8_t> decryptedToc;  // Pre-decrypted TOC for fast access
        bool tocDecrypted = false;
    };
    static std::unordered_map<std::string, std::unique_ptr<RpfStreamInfo>> g_rpfStreams;
    
    // Map any handle to its backing RPF name (e.g., "common", "xbox360", "audio")
    static std::unordered_map<uint32_t, std::string> g_handleToRpf;
    
    // Offset-to-file mapping for serving extracted files
    struct RpfFileEntry {
        uint32_t offset;
        uint32_t size;
        const char* path;
    };
    
    // File mapping for common.rpf - maps RPF offsets to extracted file paths
    // Generated from parsing the decrypted TOC
    static const RpfFileEntry g_commonRpfFiles[] = {
        {0x005000,   469881, "data/action_table.csv"},
        {0x00C800,   123153, "data/ambient.dat"},
        {0x010000,    12063, "data/animgrp.dat"},
        {0x010800,    24102, "data/carcols.dat"},
        {0x012800,     5554, "data/cargrp.dat"},
        {0x013000,    22853, "data/credits_360.dat"},
        {0x0152FE,      882, "data/default.dat"},
        {0x015800,    11342, "data/default.ide"},
        {0x016800,     5534, "data/femaleplayersettings.dat"},
        {0x017000,     9855, "data/fonts.dat"},
        {0x018000,    13883, "data/frontend_360.dat"},
        {0x019000,    36821, "data/frontend_menus.xml"},
        {0x01A000,     8022, "data/gta.dat"},
        {0x01B000,    41386, "data/handling.dat"},
        {0x01D800,     9498, "data/hud.dat"},
        {0x01E000,     4529, "data/hudcolor.dat"},
        {0x01E800,     5964, "data/images.txt"},
        {0x01F000,    18884, "data/info.zon"},
        {0x020800,     3779, "data/introspline.csv"},
        {0x021000,    77653, "data/leaderboards_data.xml"},
        {0x022000,    14880, "data/loadingscreens_360.dat"},
        {0x023000,     4828, "data/maleplayersettings.dat"},
        {0x024000,     9047, "data/meleeanims.dat"},
        {0x025000,     7988, "data/moveblend.dat"},
        {0x025800,      974, "data/nav.dat"},
        {0x026000,   139863, "data/navprecalc.dat"},
        {0x03B000,     6873, "data/networktest.dat"},
        {0x03C000,     8735, "data/pedgrp.dat"},
        {0x03D000,    89589, "data/pedpersonality.dat"},
        {0x03E800,    96255, "data/peds.ide"},
        {0x041000,   158690, "data/pedvariations.dat"},
        {0x043000,    10774, "data/plants.dat"},
        {0x044000,   594605, "data/popcycle.dat"},
        {0x048000,   593598, "data/popcycle.datnew"},
        {0x04C800,    10800, "data/precincts.dat"},
        {0x04D000,     3490, "data/radiohud.dat"},
        {0x04D800,     1393, "data/radiologo.dat"},
        {0x04E000,     9208, "data/relationships.dat"},
        {0x04E800,    12207, "data/scenarios.dat"},
        {0x04F800,     4046, "data/scrollbars.dat"},
        {0x050000,    18052, "data/startup.sco"},
        {0x051000,     5107, "data/statexport.dat"},
        {0x052000,     6665, "data/surface.dat"},
        {0x053000,    10312, "data/surfaceaudio.dat"},
        {0x054000,     5149, "data/timecycle.dat"},
        {0x055000,     5149, "data/timecyclemod.dat"},
        {0x056000,     2451, "data/traincamnodes.txt"},
        {0x057000,     5036, "data/traintrack.dat"},
        {0x058000,    16626, "data/tuneables.dat"},
        {0x059000,    15168, "data/vehcomps.dat"},
        {0x05A800,   286690, "data/vehoff.csv"},
        {0x063000,    43058, "data/vehicles.ide"},
        {0x068000,    35451, "data/visualsettings.dat"},
        {0x06C000,    16393, "data/water.dat"},
        {0x06D000,    29497, "data/weaponinfo.xml"},
        {0x070000,    10127, "data/weapons.dat"},
        {0x072000,    15680, "data/weather.dat"},
        {0x087800,   270035, "data/maps/occlu.ipl"},
        {0x09B800,  1720829, "data/maps/paths.ipl"},
        {0x198800,    99007, "shaders/fxl_final/deferred_lighting.fxc"},
        {0x19F800,    38797, "shaders/fxl_final/gta_atmoscatt_clouds.fxc"},
        {0x1A2800,    29220, "shaders/fxl_final/gta_cubemap_reflect.fxc"},
        {0x1A5000,    28838, "shaders/fxl_final/gta_cutout_fence.fxc"},
        {0x1A7800,    29201, "shaders/fxl_final/gta_decal.fxc"},
        {0x1AA000,    29122, "shaders/fxl_final/gta_decal_amb_only.fxc"},
        {0x1AC800,    28356, "shaders/fxl_final/gta_decal_dirt.fxc"},
        {0x1AF000,    29385, "shaders/fxl_final/gta_decal_glue.fxc"},
        {0x1B1800,    31357, "shaders/fxl_final/gta_decal_normal_only.fxc"},
        {0x1B4800,    28758, "shaders/fxl_final/gta_default.fxc"},
        {0x1B7000,    17697, "shaders/fxl_final/gta_diffuse_instance.fxc"},
        {0x1B8800,    20703, "shaders/fxl_final/gta_emissive.fxc"},
        {0x1BA000,    21405, "shaders/fxl_final/gta_emissivenight.fxc"},
        {0x1BB800,    20965, "shaders/fxl_final/gta_emissivestrong.fxc"},
        {0x1BD000,    31661, "shaders/fxl_final/gta_glass.fxc"},
        {0x1C0000,    23504, "shaders/fxl_final/gta_glass_emissive.fxc"},
        {0x1C2000,    24048, "shaders/fxl_final/gta_glass_emissivenight.fxc"},
        {0x1C4000,    33090, "shaders/fxl_final/gta_glass_normal_spec_reflect.fxc"},
        {0x1C7000,    30062, "shaders/fxl_final/gta_glass_reflect.fxc"},
        {0x1C9800,    29649, "shaders/fxl_final/gta_glass_spec.fxc"},
        {0x1CC800,    23995, "shaders/fxl_final/gta_hair_sorted_alpha.fxc"},
        {0x1CF000,    23882, "shaders/fxl_final/gta_hair_sorted_alpha_exp.fxc"},
        {0x1D1800,    14196, "shaders/fxl_final/gta_im.fxc"},
        {0x1D3000,    31405, "shaders/fxl_final/gta_normal.fxc"},
        {0x1D6000,    31219, "shaders/fxl_final/gta_normal_cubemap_reflect.fxc"},
        {0x1D9000,    31454, "shaders/fxl_final/gta_normal_decal.fxc"},
        {0x1DC000,    30991, "shaders/fxl_final/gta_normal_reflect.fxc"},
        {0x1DE800,    31123, "shaders/fxl_final/gta_normal_reflect_alpha.fxc"},
        {0x1E1000,    31044, "shaders/fxl_final/gta_normal_reflect_decal.fxc"},
        {0x1E3800,    32750, "shaders/fxl_final/gta_normal_spec.fxc"},
        {0x1E6800,    34062, "shaders/fxl_final/gta_normal_spec_cubemap_reflect.fxc"},
        {0x1E9800,    32791, "shaders/fxl_final/gta_normal_spec_decal.fxc"},
        {0x1EC800,    35048, "shaders/fxl_final/gta_normal_spec_reflect.fxc"},
        {0x1EF800,    35109, "shaders/fxl_final/gta_normal_spec_reflect_decal.fxc"},
        {0x1F2800,    24134, "shaders/fxl_final/gta_normal_spec_reflect_emissive.fxc"},
        {0x1F4800,    24686, "shaders/fxl_final/gta_normal_spec_reflect_emissivenight.fxc"},
        {0x1F6800,    32284, "shaders/fxl_final/gta_parallax.fxc"},
        {0x1F9800,    33573, "shaders/fxl_final/gta_parallax_specmap.fxc"},
        {0x1FC800,    32784, "shaders/fxl_final/gta_parallax_steep.fxc"},
        {0x1FF800,    24963, "shaders/fxl_final/gta_ped.fxc"},
        {0x202000,    26468, "shaders/fxl_final/gta_ped_reflect.fxc"},
        {0x204800,    25642, "shaders/fxl_final/gta_ped_skin.fxc"},
        {0x207000,    32242, "shaders/fxl_final/gta_ped_skin_blendshape.fxc"},
        {0x20A000,    42311, "shaders/fxl_final/gta_projtex.fxc"},
        {0x20E000,    42775, "shaders/fxl_final/gta_projtex_steep.fxc"},
        {0x212800,    28972, "shaders/fxl_final/gta_reflect.fxc"},
        {0x215000,    28969, "shaders/fxl_final/gta_reflect_decal.fxc"},
        {0x21F800,    31551, "shaders/fxl_final/gta_spec.fxc"},
        {0x222800,    31572, "shaders/fxl_final/gta_spec_decal.fxc"},
        {0x225800,    32083, "shaders/fxl_final/gta_spec_reflect.fxc"},
        {0x228800,    32116, "shaders/fxl_final/gta_spec_reflect_decal.fxc"},
        {0x22B800,    12493, "shaders/fxl_final/gta_terrain_va_2lyr.fxc"},
        {0x22C800,    16869, "shaders/fxl_final/gta_terrain_va_3lyr.fxc"},
        {0x22D800,    21225, "shaders/fxl_final/gta_terrain_va_4lyr.fxc"},
        {0x22E800,    11423, "shaders/fxl_final/gta_trees.fxc"},
        {0x22F800,    39294, "shaders/fxl_final/gta_vehicle_badges.fxc"},
        {0x232800,    31620, "shaders/fxl_final/gta_vehicle_basic.fxc"},
        {0x235000,    43225, "shaders/fxl_final/gta_vehicle_chrome.fxc"},
        {0x238800,    28625, "shaders/fxl_final/gta_vehicle_disc.fxc"},
        {0x23A800,    43656, "shaders/fxl_final/gta_vehicle_generic.fxc"},
        {0x23E000,    42850, "shaders/fxl_final/gta_vehicle_interior.fxc"},
        {0x241800,    41461, "shaders/fxl_final/gta_vehicle_interior2.fxc"},
        {0x245000,    45261, "shaders/fxl_final/gta_vehicle_lightsemissive.fxc"},
        {0x249000,    44350, "shaders/fxl_final/gta_vehicle_mesh.fxc"},
        {0x24C800,    43525, "shaders/fxl_final/gta_vehicle_paint1.fxc"},
        {0x250000,    43269, "shaders/fxl_final/gta_vehicle_paint2.fxc"},
        {0x253800,    44582, "shaders/fxl_final/gta_vehicle_paint3.fxc"},
        {0x257000,    32462, "shaders/fxl_final/gta_vehicle_rims1.fxc"},
        {0x259000,    32134, "shaders/fxl_final/gta_vehicle_rims2.fxc"},
        {0x25B000,    37859, "shaders/fxl_final/gta_vehicle_rubber.fxc"},
        {0x25E000,    37472, "shaders/fxl_final/gta_vehicle_shuts.fxc"},
        {0x261000,    33297, "shaders/fxl_final/gta_vehicle_tire.fxc"},
        {0x263800,    39399, "shaders/fxl_final/gta_vehicle_vehglass.fxc"},
        {0x267000,    29706, "shaders/fxl_final/gta_wire.fxc"},
        {0x26E000,    36486, "shaders/fxl_final/rage_postfx.fxc"},
        {0x273000,    10514, "shaders/fxl_final/water.fxc"},
        {0x275000,  2767809, "text/american.gxt"},
        {0x519000,  3034116, "text/french.gxt"},
        {0x7FE000,  3048486, "text/german.gxt"},
        {0xAE6800,  2915066, "text/italian.gxt"},
        {0xDAE800,  2876898, "text/spanish.gxt"},
        {0, 0, nullptr}  // Sentinel
    };
    
    // Find extracted file for a given RPF offset
    // Since RPF files can have overlapping ranges (due to compression), we need to find
    // the file whose start offset is closest to (but not greater than) the target offset
    static const RpfFileEntry* FindExtractedFile(const std::string& rpfName, uint32_t offset)
    {
        const RpfFileEntry* entries = nullptr;
        if (rpfName == "common")
            entries = g_commonRpfFiles;
        // TODO: Add xbox360 and audio mappings
        
        if (!entries)
            return nullptr;
        
        const RpfFileEntry* bestMatch = nullptr;
        uint32_t bestDistance = UINT32_MAX;
        
        for (const RpfFileEntry* e = entries; e->path != nullptr; ++e)
        {
            // File must start at or before the target offset
            if (e->offset <= offset)
            {
                // Check if target is within this file's range
                if (offset < e->offset + e->size)
                {
                    // Find the file with the smallest distance (closest start offset)
                    uint32_t distance = offset - e->offset;
                    if (distance < bestDistance)
                    {
                        bestDistance = distance;
                        bestMatch = e;
                    }
                }
            }
        }
        return bestMatch;
    }
    
    // Cache for opened extracted files
    struct ExtractedFileCache {
        std::fstream stream;
        std::string path;
        uint32_t rpfOffset;
        uint32_t fileSize;
    };
    static std::unordered_map<std::string, std::unique_ptr<ExtractedFileCache>> g_extractedFileCache;
    
    // Get or open an RPF stream
    static RpfStreamInfo* GetRpfStream(const std::string& rpfName)
    {
        auto it = g_rpfStreams.find(rpfName);
        if (it != g_rpfStreams.end())
            return it->second.get();
        
        // Open the RPF file
        std::filesystem::path rpfPath = GetGamePath() / "game" / (rpfName + ".rpf");
        if (!std::filesystem::exists(rpfPath))
        {
            LOGF_IMPL(Utility, "GetRpfStream", "RPF not found: {}", rpfPath.string());
            return nullptr;
        }
        
        auto info = std::make_unique<RpfStreamInfo>();
        info->stream.open(rpfPath, std::ios::in | std::ios::binary);
        if (!info->stream.is_open())
        {
            LOGF_IMPL(Utility, "GetRpfStream", "Failed to open RPF: {}", rpfPath.string());
            return nullptr;
        }
        
        info->path = rpfPath;
        {
            info->stream.clear();
            info->stream.seekg(0, std::ios::end);
            const std::streampos endPos = info->stream.tellg();
            if (endPos > 0)
                info->fileSize = static_cast<uint64_t>(endPos);
            info->stream.clear();
            info->stream.seekg(0, std::ios::beg);
        }
        
        // Parse RPF2 header
        char hdr[20] = {};
        info->stream.read(hdr, sizeof(hdr));
        if (info->stream.gcount() >= 20)
        {
            bool isRpf2 = (hdr[0] == 'R' && hdr[1] == 'P' && hdr[2] == 'F' && hdr[3] == '2');
            if (isRpf2)
            {
                auto le32 = [](const uint8_t* p) -> uint32_t {
                    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
                };
                info->tocSize = le32(reinterpret_cast<uint8_t*>(hdr + 4));
                uint32_t encrypted = le32(reinterpret_cast<uint8_t*>(hdr + 16));
                info->tocOffset = 0x800;
                info->tocEncrypted = (encrypted != 0);
                info->headerParsed = true;
                
                LOGF_IMPL(Utility, "GetRpfStream", "Opened {} - tocOffset=0x{:X} tocSize={} encrypted={}",
                          rpfName, info->tocOffset, info->tocSize, info->tocEncrypted);
            }
        }
        info->stream.clear();
        info->stream.seekg(0, std::ios::beg);
        
        RpfStreamInfo* ptr = info.get();
        g_rpfStreams[rpfName] = std::move(info);
        return ptr;
    }

    static void DecryptRpfBufferInPlace(uint8_t* data, uint32_t length, uint64_t tocRelativeOffset);
    
    static void EnsureTocDecrypted(RpfStreamInfo* rpf)
    {
        if (!rpf || rpf->tocDecrypted || !rpf->headerParsed || !rpf->tocEncrypted)
            return;
        if (rpf->tocSize == 0)
            return;
        
        rpf->decryptedToc.resize(rpf->tocSize);
        rpf->stream.clear();
        rpf->stream.seekg(rpf->tocOffset, std::ios::beg);
        rpf->stream.read(reinterpret_cast<char*>(rpf->decryptedToc.data()), rpf->tocSize);
        
        if (rpf->stream.gcount() != static_cast<std::streamsize>(rpf->tocSize))
        {
            LOGF_IMPL(Utility, "EnsureTocDecrypted", "Failed to read TOC: got {} bytes, expected {}", 
                      rpf->stream.gcount(), rpf->tocSize);
            rpf->decryptedToc.clear();
            return;
        }
        
        DecryptRpfBufferInPlace(rpf->decryptedToc.data(), rpf->tocSize, 0);
        rpf->tocDecrypted = true;
        LOGF_IMPL(Utility, "EnsureTocDecrypted", "Pre-decrypted {} bytes of TOC", rpf->tocSize);
    }

    // Read from extracted file given RPF name and offset
    static uint32_t ReadFromExtractedFile(const std::string& rpfName, uint8_t* hostBuffer, uint32_t size, uint32_t offset, int count)
    {
        const RpfFileEntry* entry = FindExtractedFile(rpfName, offset);
        if (!entry)
            return 0;
        
        // Calculate offset within the extracted file
        const uint32_t fileOffset = offset - entry->offset;
        if (fileOffset >= entry->size)
            return 0;
        
        // Build path to extracted file
        std::filesystem::path extractedPath = GetGamePath() / "extracted" / rpfName / entry->path;
        
        // Check cache first
        auto cacheKey = extractedPath.string();
        auto cacheIt = g_extractedFileCache.find(cacheKey);
        ExtractedFileCache* cache = nullptr;
        
        if (cacheIt != g_extractedFileCache.end())
        {
            cache = cacheIt->second.get();
        }
        else
        {
            // Open the extracted file
            if (!std::filesystem::exists(extractedPath))
            {
                if (count <= 20)
                {
                    LOGF_IMPL(Utility, "GTA4_FileLoad", "Extracted file not found: {}", extractedPath.string());
                }
                return 0;
            }
            
            auto newCache = std::make_unique<ExtractedFileCache>();
            newCache->stream.open(extractedPath, std::ios::in | std::ios::binary);
            if (!newCache->stream.is_open())
            {
                if (count <= 20)
                {
                    LOGF_IMPL(Utility, "GTA4_FileLoad", "Failed to open extracted file: {}", extractedPath.string());
                }
                return 0;
            }
            
            newCache->path = cacheKey;
            newCache->rpfOffset = entry->offset;
            newCache->fileSize = entry->size;
            
            cache = newCache.get();
            g_extractedFileCache[cacheKey] = std::move(newCache);
            
            if (count <= 50)
            {
                LOGF_IMPL(Utility, "GTA4_FileLoad", "Opened extracted file: {} (rpfOffset=0x{:X} size={})",
                          entry->path, entry->offset, entry->size);
            }
        }
        
        // Read from the extracted file
        cache->stream.clear();
        cache->stream.seekg(fileOffset, std::ios::beg);
        
        const uint32_t availableInFile = entry->size - fileOffset;
        const uint32_t toRead = std::min(size, availableInFile);
        
        cache->stream.read(reinterpret_cast<char*>(hostBuffer), toRead);
        const uint32_t bytesRead = static_cast<uint32_t>(cache->stream.gcount());
        
        if (count <= 50 || count % 100 == 0)
        {
            LOGF_IMPL(Utility, "GTA4_FileLoad", "Read {} bytes from extracted '{}' at fileOffset=0x{:X}",
                      bytesRead, entry->path, fileOffset);
        }
        
        return bytesRead;
    }
    
    static uint32_t ReadFromRpfStream(RpfStreamInfo* rpf, uint8_t* hostBuffer, uint32_t size, uint32_t offset, int count, const std::string& rpfName)
    {
        if (!rpf || !rpf->stream.is_open())
            return 0;
        if (rpf->fileSize != 0 && offset >= rpf->fileSize)
            return 0;
        
        // Ensure TOC is pre-decrypted for fast access
        EnsureTocDecrypted(rpf);
        
        // Check if this read overlaps with the TOC region
        const uint64_t tocStart = rpf->tocOffset;
        const uint64_t tocEnd = tocStart + rpf->tocSize;
        const uint64_t readStart = offset;
        const uint64_t readEnd = offset + size;
        
        // If we have a pre-decrypted TOC and the read overlaps it, serve from cache
        if (rpf->tocDecrypted && !rpf->decryptedToc.empty() && readEnd > tocStart && readStart < tocEnd)
        {
            // Read overlaps TOC - serve decrypted data
            uint32_t totalRead = 0;
            
            // Part before TOC (if any)
            if (readStart < tocStart)
            {
                const uint32_t beforeLen = static_cast<uint32_t>(tocStart - readStart);
                rpf->stream.clear();
                rpf->stream.seekg(offset, std::ios::beg);
                rpf->stream.read(reinterpret_cast<char*>(hostBuffer), beforeLen);
                totalRead += static_cast<uint32_t>(rpf->stream.gcount());
            }
            
            // TOC overlap part - serve from decrypted cache
            const uint64_t overlapStart = std::max(readStart, tocStart);
            const uint64_t overlapEnd = std::min(readEnd, tocEnd);
            if (overlapEnd > overlapStart)
            {
                const uint32_t tocOff = static_cast<uint32_t>(overlapStart - tocStart);
                const uint32_t bufOffset = static_cast<uint32_t>(overlapStart - readStart);
                const uint32_t len = static_cast<uint32_t>(overlapEnd - overlapStart);
                
                memcpy(hostBuffer + bufOffset, rpf->decryptedToc.data() + tocOff, len);
                totalRead += len;
                
                if (count <= 20 || count % 100 == 0)
                {
                    LOGF_IMPL(Utility, "GTA4_FileLoad", "Served {} bytes from pre-decrypted TOC at tocOffset=0x{:X}",
                        len, tocOff);
                }
            }
            
            // Part after TOC - serve raw RPF data (not extracted text files)
            if (readEnd > tocEnd)
            {
                const uint32_t afterOffset = static_cast<uint32_t>(tocEnd);
                const uint32_t afterLen = static_cast<uint32_t>(readEnd - tocEnd);
                const uint32_t bufOffset = static_cast<uint32_t>(tocEnd - readStart);
                
                // Read raw RPF data directly - extracted files are TEXT format, game expects BINARY
                rpf->stream.clear();
                rpf->stream.seekg(afterOffset, std::ios::beg);
                rpf->stream.read(reinterpret_cast<char*>(hostBuffer + bufOffset), afterLen);
                totalRead += static_cast<uint32_t>(rpf->stream.gcount());
            }
            
            return totalRead;
        }
        
        // For reads past TOC, read raw RPF data directly
        // NOTE: Extracted files are TEXT format, but game expects BINARY RPF format
        
        // Read from raw RPF
        rpf->stream.clear();
        rpf->stream.seekg(offset, std::ios::beg);
        
        if (rpf->stream.bad())
            return 0;
        
        rpf->stream.read(reinterpret_cast<char*>(hostBuffer), size);
        const uint32_t bytesRead = static_cast<uint32_t>(rpf->stream.gcount());
        
        if (bytesRead == 0)
            return 0;
        
        // Fallback: decrypt TOC region on-the-fly if not using cache
        if (rpf->headerParsed && rpf->tocEncrypted && !rpf->tocDecrypted)
        {
            const uint64_t overlapStart = std::max(readStart, tocStart);
            const uint64_t overlapEnd = std::min(static_cast<uint64_t>(offset + bytesRead), tocEnd);
            
            if (overlapEnd > overlapStart)
            {
                const uint32_t startInBuf = static_cast<uint32_t>(overlapStart - readStart);
                const uint32_t len = static_cast<uint32_t>(overlapEnd - overlapStart);
                const uint64_t tocRelativeOffset = overlapStart - tocStart;
                
                if (count <= 20 || count % 100 == 0)
                {
                    LOGF_IMPL(Utility, "GTA4_FileLoad", "Decrypting TOC region on-the-fly: bufOffset={} len={} tocRelOffset={}",
                        startInBuf, len, tocRelativeOffset);
                }
                
                DecryptRpfBufferInPlace(hostBuffer + startInBuf, len, tocRelativeOffset);
            }
        }
        
        return bytesRead;
    }

    static uint32_t ReadFromBestRpf(uint32_t handle, uint8_t* hostBuffer, uint32_t size, uint32_t offset, int count, std::string& outName)
    {
        static const std::string kCommon = "common";
        static const std::string kXbox360 = "xbox360";
        static const std::string kAudio = "audio";
        
        auto tryName = [&](const std::string& name) -> uint32_t {
            RpfStreamInfo* rpf = GetRpfStream(name);
            if (!rpf)
                return 0;
            if (rpf->fileSize != 0 && offset >= rpf->fileSize)
                return 0;
            return ReadFromRpfStream(rpf, hostBuffer, size, offset, count, name);
        };
        
        if (auto mapIt = g_handleToRpf.find(handle); mapIt != g_handleToRpf.end())
        {
            const std::string mapped = mapIt->second;
            if (uint32_t n = tryName(mapped); n != 0)
            {
                outName = mapped;
                return n;
            }
            g_handleToRpf.erase(mapIt);
        }
        
        if (uint32_t n = tryName(kCommon); n != 0) { outName = kCommon; return n; }
        if (uint32_t n = tryName(kXbox360); n != 0) { outName = kXbox360; return n; }
        if (uint32_t n = tryName(kAudio); n != 0) { outName = kAudio; return n; }
        
        return 0;
    }

    static std::vector<uint8_t> MakeEmptyRpfImage()
    {
        // Many GTA IV code paths treat a trailing-slash open (e.g. "game:\\") as
        // "mount the disc/RPF container" and then read the RPF header.
        //
        // Provide a tiny, valid-looking header so the title can proceed without
        // triggering Dirty Disc, while we serve actual assets from extracted files.
        //
        // Header layout (20 bytes):
        //   magic[4] = 'RPF3' (bytes)
        //   toc_size u32
        //   entry_count u32
        //   unknown u32
        //   encrypted u32
        // We set all numeric fields to 0 (endian-agnostic).
        std::vector<uint8_t> buf(0x800, 0);
        buf[0] = 'R';
        buf[1] = 'P';
        buf[2] = 'F';
        buf[3] = '3';
        return buf;
    }

    static uint32_t AlignUp(uint32_t value, uint32_t align)
    {
        return (value + (align - 1)) & ~(align - 1);
    }

    // AES key for RPF TOC decryption (loaded from RPF DUMP/aes_key.bin)
    static std::vector<uint8_t> g_aesKey;

    static void LoadAesKeyIfNeeded()
    {
        if (!g_aesKey.empty())
            return;

        const auto keyPath = GetGamePath() / "RPF DUMP" / "aes_key.bin";
        std::error_code ec;
        if (!std::filesystem::exists(keyPath, ec))
        {
            LOGF_IMPL(Utility, "LoadAesKeyIfNeeded", "AES key not found: '{}'", keyPath.string());
            return;
        }

        std::ifstream f(keyPath, std::ios::binary);
        if (!f.is_open())
        {
            LOGF_IMPL(Utility, "LoadAesKeyIfNeeded", "Failed to open AES key: '{}'", keyPath.string());
            return;
        }

        std::vector<uint8_t> key((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        if (key.size() >= 32)
        {
            g_aesKey.assign(key.begin(), key.begin() + 32);
            LOGF_IMPL(Utility, "LoadAesKeyIfNeeded", "Loaded AES key ({} bytes)", (int)g_aesKey.size());
        }
        else
        {
            LOGF_IMPL(Utility, "LoadAesKeyIfNeeded", "AES key too small: {} bytes", (int)key.size());
        }
    }

    // Decrypt RPF TOC data using AES-256 ECB repeated 16 times (SparkIV behavior).
    // This version handles misaligned buffer starts by only decrypting complete 16-byte blocks.
    // The caller is responsible for ensuring the buffer corresponds to aligned TOC offsets.
    static void DecryptRpfBufferInPlace(uint8_t* data, uint32_t length, uint64_t tocRelativeOffset = 0)
    {
        if (!data || length == 0)
            return;

        LoadAesKeyIfNeeded();
        if (g_aesKey.size() != 32)
        {
            LOGF_IMPL(Utility, "DecryptRpfBufferInPlace", "AES key not available (size={})", g_aesKey.size());
            return;
        }

        // AES-ECB works on 16-byte blocks. We need to handle the case where the read
        // doesn't start on a block boundary within the TOC.
        // 
        // Strategy: Find the first complete 16-byte block boundary in the buffer,
        // decrypt from there, and leave any partial leading/trailing bytes as-is.
        // This is safe because the game will typically read aligned chunks.
        
        const uint32_t blockSize = 16;
        
        // Calculate how many bytes into a block we are (based on TOC-relative offset)
        uint32_t offsetIntoBlock = static_cast<uint32_t>(tocRelativeOffset % blockSize);
        
        // Skip leading partial block bytes
        uint32_t skipBytes = (offsetIntoBlock == 0) ? 0 : (blockSize - offsetIntoBlock);
        if (skipBytes >= length)
        {
            // Entire buffer is within a partial block, can't decrypt
            LOGF_IMPL(Utility, "DecryptRpfBufferInPlace", "Buffer too small for aligned decrypt (len={} skip={})", length, skipBytes);
            return;
        }
        
        uint8_t* alignedStart = data + skipBytes;
        uint32_t remainingLen = length - skipBytes;
        uint32_t alignedLen = remainingLen & ~0x0Fu; // Round down to block boundary
        
        if (alignedLen == 0)
        {
            LOGF_IMPL(Utility, "DecryptRpfBufferInPlace", "No complete blocks to decrypt (len={} skip={})", length, skipBytes);
            return;
        }

#if defined(__APPLE__)
        std::vector<uint8_t> tmp(alignedLen);

        // Repeat AES-ECB-256 decrypt 16 times (matches SparkIV behavior)
        for (int iter = 0; iter < 16; ++iter)
        {
            size_t outLen = 0;
            CCCryptorStatus status = CCCrypt(kCCDecrypt,
                                             kCCAlgorithmAES,
                                             kCCOptionECBMode,
                                             g_aesKey.data(), kCCKeySizeAES256,
                                             nullptr,
                                             alignedStart, alignedLen,
                                             tmp.data(), alignedLen,
                                             &outLen);

            if (status != kCCSuccess || outLen != alignedLen)
            {
                LOGF_IMPL(Utility, "DecryptRpfBufferInPlace", "AES decrypt failed (status={} iter={})", status, iter);
                return;
            }

            // Copy back for next iteration or result
            memcpy(alignedStart, tmp.data(), alignedLen);
        }
        
        LOGF_IMPL(Utility, "DecryptRpfBufferInPlace", "Decrypted {} bytes (skipped {} leading bytes)", alignedLen, skipBytes);
#else
        // Non-macOS: no AES implementation available here.
        LOGF_IMPL(Utility, "DecryptRpfBufferInPlace", "AES decrypt not implemented on this platform");
#endif
    }

    static uint32_t ErrnoToNtStatus(int err)
    {
        switch (err)
        {
        case EACCES:
            return STATUS_ACCESS_DENIED;
        case ENOENT:
            return STATUS_OBJECT_NAME_NOT_FOUND;
        case ENOTDIR:
            return STATUS_OBJECT_PATH_NOT_FOUND;
        default:
            return STATUS_FAIL_CHECK;
        }
    }

    static void ParseRpfHeader(NtFileHandle* h)
    {
        if (!h || h->rpfHeaderParsed)
            return;

        h->rpfHeaderParsed = true;

        try {
            const std::string ext = h->path.extension().string();
            std::string extLower = ext;
            std::transform(extLower.begin(), extLower.end(), extLower.begin(), [](unsigned char c){ return std::tolower(c); });
            if (extLower != ".rpf")
                return;

            h->isRpf = true;

            // Read first 20 bytes of header
            std::streampos cur = h->stream.tellg();
            h->stream.clear();
            h->stream.seekg(0, std::ios::beg);
            char hdr[20] = {};
            h->stream.read(hdr, sizeof(hdr));
            const std::streamsize got = h->stream.gcount();
            // restore position
            h->stream.clear();
            h->stream.seekg(cur, std::ios::beg);

            if (got < 20)
            {
                LOGF_IMPL(Utility, "ParseRpfHeader", "Failed to read header from '{}' (got {} bytes)", h->path.string(), got);
                return;
            }

            // GTA IV Xbox 360 uses RPF2 (0x52504632 = "RPF2"), not RPF3
            // RPF2 Header layout (20 bytes):
            //   4b - INT32 - RPF Version (0x52504632 for RPF2)
            //   4b - INT32 - Table of Contents Size
            //   4b - INT32 - Number of Entries
            //   4b - INT32 - Unknown
            //   4b - INT32 - Encrypted (0 = unencrypted, non-zero = encrypted)
            
            // Check magic - accept both RPF2 and RPF3
            bool isRpf2 = (hdr[0] == 'R' && hdr[1] == 'P' && hdr[2] == 'F' && hdr[3] == '2');
            bool isRpf3 = (hdr[0] == 'R' && hdr[1] == 'P' && hdr[2] == 'F' && hdr[3] == '3');
            
            if (!isRpf2 && !isRpf3)
            {
                LOGF_IMPL(Utility, "ParseRpfHeader", "Unknown RPF magic in '{}': {:02X} {:02X} {:02X} {:02X}", 
                          h->path.string(), (uint8_t)hdr[0], (uint8_t)hdr[1], (uint8_t)hdr[2], (uint8_t)hdr[3]);
                return;
            }

            auto le32 = [](const uint8_t* p) -> uint32_t {
                return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
            };

            uint32_t tocSize = le32(reinterpret_cast<uint8_t*>(hdr + 4));
            uint32_t numEntries = le32(reinterpret_cast<uint8_t*>(hdr + 8));
            uint32_t encrypted = le32(reinterpret_cast<uint8_t*>(hdr + 16));

            // TOC starts at offset 0x800 (2048 bytes from file origin)
            h->tocOffset = 0x800;
            h->tocSize = tocSize;
            h->tocEncrypted = (encrypted != 0);

            LOGF_IMPL(Utility, "ParseRpfHeader", "Parsed {} header '{}': tocOffset=0x{:X} tocSize={} entries={} encrypted={}", 
                      isRpf2 ? "RPF2" : "RPF3", h->path.filename().string(), h->tocOffset, tocSize, numEntries, h->tocEncrypted);
        }
        catch (...) {
            LOGF_IMPL(Utility, "ParseRpfHeader", "Exception parsing header for '{}'", h->path.string());
        }
    }

    static bool TryGetAnsiPath(const XOBJECT_ATTRIBUTES* attributes, std::string& out)
    {
        out.clear();
        if (!attributes)
        {
            LOG_IMPL(Utility, "TryGetAnsiPath", "attributes is null");
            return false;
        }

        const XANSI_STRING* name = attributes->Name.get();
        if (!name)
        {
            LOG_IMPL(Utility, "TryGetAnsiPath", "Name.get() is null");
            return false;
        }

        const char* buf = name->Buffer.get();
        if (!buf)
        {
            LOG_IMPL(Utility, "TryGetAnsiPath", "Buffer.get() is null");
            return false;
        }

        const uint16_t len = name->Length.get();
        // Sanity check: zero length or very large lengths are suspect
        if (len == 0)
        {
            // Empty paths will always fail - reject silently
            return false;
        }
        else if (len > 1024)
        {
            LOGF_IMPL(Utility, "TryGetAnsiPath", "Suspicious length: {} (max 1024)", len);
            return false;
        }
        else
        {
            // Check if the path looks valid
            // Valid paths should start with a letter (drive:) or a letter after game:\ prefix
            unsigned char firstChar = static_cast<unsigned char>(buf[0]);
            
            // First character should be printable ASCII for a valid path
            // Valid starts: letters (a-z, A-Z), backslash, dot, or space
            bool validStart = (firstChar >= 'A' && firstChar <= 'Z') ||
                              (firstChar >= 'a' && firstChar <= 'z') ||
                              firstChar == '\\' || firstChar == '.' || 
                              firstChar == ' ' || firstChar == '/';
            
            if (!validStart)
            {
                // Check if it's just high-byte garbage (common pattern)
                if (firstChar >= 128)
                {
                    LOGF_IMPL(Utility, "TryGetAnsiPath", "Garbage path rejected (len={}, first=0x{:02X}, Name.ptr=0x{:08X}, Buffer.ptr=0x{:08X})", 
                        len, firstChar, attributes->Name.ptr.value, name->Buffer.ptr.value);
                    return false;
                }
            }
            
            // Also check for excessive non-ASCII content
            size_t garbageCount = 0;
            for (size_t i = 0; i < std::min<size_t>(len, 20); i++)
            {
                unsigned char c = static_cast<unsigned char>(buf[i]);
                if (c >= 128)
                    garbageCount++;
            }
            // If more than 50% of first 20 chars are non-ASCII, reject
            if (garbageCount > 10)
            {
                LOGF_IMPL(Utility, "TryGetAnsiPath", "Mostly garbage path rejected (len={}, garbage={}/20)", len, garbageCount);
                return false;
            }
        }

        out.assign(buf, buf + len);
        return true;
    }

    static std::filesystem::path ResolveGuestPathBestEffort(const std::string& guestPath)
    {
        std::filesystem::path resolved = FileSystem::ResolvePath(guestPath, true);

        std::error_code ec;
        if (std::filesystem::exists(resolved, ec))
            return resolved;

        const std::filesystem::path cachedPath = FindInPathCache(resolved.string());
        if (!cachedPath.empty())
            return cachedPath;

        return resolved;
    }

    static uint32_t GetFileAttributesBestEffort(const std::filesystem::path& path)
    {
        std::error_code ec;
        const bool isDir = std::filesystem::is_directory(path, ec);
        if (ec)
            return FILE_ATTRIBUTE_NORMAL;

        if (isDir)
            return FILE_ATTRIBUTE_DIRECTORY;

        return FILE_ATTRIBUTE_NORMAL;
    }

    static uint64_t RoundUpToPage(uint64_t value)
    {
        constexpr uint64_t kPageSize = 0x1000;
        return (value + (kPageSize - 1)) & ~(kPageSize - 1);
    }

#pragma pack(push, 1)
    struct XFILE_BASIC_INFORMATION
    {
        be<int64_t> CreationTime;
        be<int64_t> LastAccessTime;
        be<int64_t> LastWriteTime;
        be<int64_t> ChangeTime;
        be<uint32_t> FileAttributes;
        be<uint32_t> Reserved;
    };

    struct XFILE_STANDARD_INFORMATION
    {
        be<int64_t> AllocationSize;
        be<int64_t> EndOfFile;
        be<uint32_t> NumberOfLinks;
        uint8_t DeletePending;
        uint8_t Directory;
        uint16_t Reserved;
    };

    struct XFILE_POSITION_INFORMATION
    {
        be<int64_t> CurrentByteOffset;
    };

    struct XFILE_END_OF_FILE_INFORMATION
    {
        be<int64_t> EndOfFile;
    };

    struct XFILE_NETWORK_OPEN_INFORMATION
    {
        be<int64_t> CreationTime;
        be<int64_t> LastAccessTime;
        be<int64_t> LastWriteTime;
        be<int64_t> ChangeTime;
        be<int64_t> AllocationSize;
        be<int64_t> EndOfFile;
        be<uint32_t> FileAttributes;
        be<uint32_t> Reserved;
    };

    struct XFILE_FS_SIZE_INFORMATION
    {
        be<int64_t> TotalAllocationUnits;
        be<int64_t> AvailableAllocationUnits;
        be<uint32_t> SectorsPerAllocationUnit;
        be<uint32_t> BytesPerSector;
    };

    struct XFILE_FS_DEVICE_INFORMATION
    {
        be<uint32_t> DeviceType;
        be<uint32_t> Characteristics;
    };

    struct XFILE_FS_ATTRIBUTE_INFORMATION_FIXED
    {
        be<uint32_t> FileSystemAttributes;
        be<uint32_t> MaximumComponentNameLength;
        be<uint32_t> FileSystemNameLength;
        be<uint16_t> FileSystemName[16];
    };

    // FILE_INFORMATION_CLASS for NtQueryDirectoryFile (subset)
    constexpr uint32_t FileDirectoryInformation = 1;
    constexpr uint32_t FileFullDirectoryInformation = 2;
    constexpr uint32_t FileBothDirectoryInformation = 3;

    struct XFILE_DIRECTORY_INFORMATION_FIXED
    {
        be<uint32_t> NextEntryOffset;
        be<uint32_t> FileIndex;
        be<int64_t> CreationTime;
        be<int64_t> LastAccessTime;
        be<int64_t> LastWriteTime;
        be<int64_t> ChangeTime;
        be<int64_t> EndOfFile;
        be<int64_t> AllocationSize;
        be<uint32_t> FileAttributes;
        be<uint32_t> FileNameLength;
        // Followed by FileName (UTF-16BE)
    };

    struct XFILE_FULL_DIR_INFORMATION_FIXED
    {
        be<uint32_t> NextEntryOffset;
        be<uint32_t> FileIndex;
        be<int64_t> CreationTime;
        be<int64_t> LastAccessTime;
        be<int64_t> LastWriteTime;
        be<int64_t> ChangeTime;
        be<int64_t> EndOfFile;
        be<int64_t> AllocationSize;
        be<uint32_t> FileAttributes;
        be<uint32_t> FileNameLength;
        be<uint32_t> EaSize;
        // Followed by FileName (UTF-16BE)
    };

    struct XFILE_BOTH_DIR_INFORMATION_FIXED
    {
        be<uint32_t> NextEntryOffset;
        be<uint32_t> FileIndex;
        be<int64_t> CreationTime;
        be<int64_t> LastAccessTime;
        be<int64_t> LastWriteTime;
        be<int64_t> ChangeTime;
        be<int64_t> EndOfFile;
        be<int64_t> AllocationSize;
        be<uint32_t> FileAttributes;
        be<uint32_t> FileNameLength;
        be<uint32_t> EaSize;
        uint8_t ShortNameLength;
        uint8_t Reserved1;
        be<uint16_t> ShortName[12];
        // Followed by FileName (UTF-16BE)
    };
#pragma pack(pop)

    static_assert(sizeof(XFILE_BASIC_INFORMATION) == 40);
    static_assert(sizeof(XFILE_STANDARD_INFORMATION) == 24);
    static_assert(sizeof(XFILE_POSITION_INFORMATION) == 8);
    static_assert(sizeof(XFILE_END_OF_FILE_INFORMATION) == 8);
    static_assert(sizeof(XFILE_NETWORK_OPEN_INFORMATION) == 56);
    static_assert(sizeof(XFILE_FS_SIZE_INFORMATION) == 24);
    static_assert(sizeof(XFILE_FS_DEVICE_INFORMATION) == 8);
}

struct Event final : KernelObject, HostObject<XKEVENT>
{
    bool manualReset;
    std::atomic<bool> signaled;

    Event(XKEVENT* header)
        : manualReset(!header->Type), signaled(!!header->SignalState)
    {
    }

    Event(bool manualReset, bool initialState)
        : manualReset(manualReset), signaled(initialState)
    {
    }

    uint32_t Wait(uint32_t timeout) override
    {
        if (timeout == 0)
        {
            if (manualReset)
            {
                if (!signaled)
                    return STATUS_TIMEOUT;
            }
            else
            {
                bool expected = true;
                if (!signaled.compare_exchange_strong(expected, false))
                    return STATUS_TIMEOUT;
            }
        }
        else if (timeout == INFINITE)
        {
            if (manualReset)
            {
                signaled.wait(false);
            }
            else
            {
                while (true)
                {
                    bool expected = true;
                    if (signaled.compare_exchange_weak(expected, false))
                        break;

                    signaled.wait(expected);
                }
            }
        }
        else
        {
            assert(false && "Unhandled timeout value.");
        }

        return STATUS_SUCCESS;
    }

    bool Set()
    {
        signaled = true;

        if (manualReset)
            signaled.notify_all();
        else
            signaled.notify_one();

        return TRUE;
    }

    bool Reset()
    {
        signaled = false;
        return TRUE;
    }
};

static std::atomic<uint32_t> g_keSetEventGeneration;

struct Semaphore final : KernelObject, HostObject<XKSEMAPHORE>
{
    std::atomic<uint32_t> count;
    uint32_t maximumCount;

    Semaphore(XKSEMAPHORE* semaphore)
        : count(semaphore->Header.SignalState), maximumCount(semaphore->Limit)
    {
    }

    Semaphore(uint32_t count, uint32_t maximumCount)
        : count(count), maximumCount(maximumCount)
    {
    }

    uint32_t Wait(uint32_t timeout) override
    {
        if (timeout == 0)
        {
            uint32_t currentCount = count.load();
            if (currentCount != 0)
            {
                if (count.compare_exchange_weak(currentCount, currentCount - 1))
                    return STATUS_SUCCESS;
            }

            return STATUS_TIMEOUT;
        }
        else if (timeout == INFINITE)
        {
            uint32_t currentCount;
            while (true)
            {
                currentCount = count.load();
                if (currentCount != 0)
                {
                    if (count.compare_exchange_weak(currentCount, currentCount - 1))
                        return STATUS_SUCCESS;
                }
                else
                {
                    count.wait(0);
                }
            }

            return STATUS_SUCCESS;
        }
        else
        {
            assert(false && "Unhandled timeout value.");
            return STATUS_TIMEOUT;
        }
    }

    void Release(uint32_t releaseCount, uint32_t* previousCount)
    {
        if (previousCount != nullptr)
            *previousCount = count;

        assert(count + releaseCount <= maximumCount);

        count += releaseCount;
        count.notify_all();
    }
};

inline void CloseKernelObject(XDISPATCHER_HEADER& header)
{
    if (header.WaitListHead.Flink != OBJECT_SIGNATURE)
    {
        return;
    }

    DestroyKernelObject(header.WaitListHead.Blink);
}

uint32_t GuestTimeoutToMilliseconds(be<int64_t>* timeout)
{
    return timeout ? (*timeout * -1) / 10000 : INFINITE;
}

void VdHSIOCalibrationLock()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeCertMonitorData()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XexExecutableModuleHandle()
{
    LOG_UTILITY("!!! STUB !!!");
}

void ExLoadedCommandLine()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeDebugMonitorData()
{
    LOG_UTILITY("!!! STUB !!!");
}

void ExThreadObjectType()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeTimeStampBundle()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XboxHardwareInfo()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XGetVideoMode()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t XGetGameRegion()
{
    if (Config::Language == ELanguage::Japanese)
        return 0x0101;

    return 0x03FF;
}

uint32_t XMsgStartIORequest(uint32_t App, uint32_t Message, XXOVERLAPPED* lpOverlapped, void* Buffer, uint32_t szBuffer)
{
    return STATUS_SUCCESS;
}

uint32_t XamUserGetSigninState(uint32_t userIndex)
{
    return true;
}

uint32_t XamGetSystemVersion()
{
    return 0;
}

void XamContentDelete()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t XamContentGetCreator(uint32_t userIndex, const XCONTENT_DATA* contentData, be<uint32_t>* isCreator, be<uint64_t>* xuid, XXOVERLAPPED* overlapped)
{
    if (isCreator)
        *isCreator = true;

    if (xuid)
        *xuid = 0xB13EBABEBABEBABE;

    return 0;
}

uint32_t XamContentGetDeviceState()
{
    return 0;
}

uint32_t XamUserGetSigninInfo(uint32_t userIndex, uint32_t flags, XUSER_SIGNIN_INFO* info)
{
    if (userIndex == 0)
    {
        memset(info, 0, sizeof(*info));
        info->xuid = 0xB13EBABEBABEBABE;
        info->SigninState = 1;
        strcpy(info->Name, "SWA");
        return 0;
    }

    return 0x00000525; // ERROR_NO_SUCH_USER
}

void XamShowSigninUI()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t XamShowDeviceSelectorUI
(
    uint32_t userIndex,
    uint32_t contentType,
    uint32_t contentFlags,
    uint64_t totalRequested,
    be<uint32_t>* deviceId,
    XXOVERLAPPED* overlapped
)
{
    XamNotifyEnqueueEvent(9, true);
    *deviceId = 1;
    XamNotifyEnqueueEvent(9, false);
    return 0;
}

void XamShowDirtyDiscErrorUI()
{
    static int callCount = 0;
    ++callCount;
    
    // Pump SDL events to keep window responsive
    PumpSdlEventsIfNeeded();
    
    // Only log first few to avoid spam
    if (callCount <= 3) {
        LOGF_UTILITY("!!! STUB !!! - Dirty disc error #{} (bypassing)", callCount);
    }
    
    // DON'T exit - just return and let the game continue
    // The game may retry or find an alternative code path
    // Small delay to prevent tight CPU loop
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // Return normally - game will either retry or continue
}

void XamEnableInactivityProcessing()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XamResetInactivity()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XamShowMessageBoxUIEx()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t XGetLanguage()
{
    return (uint32_t)Config::Language.Value;
}

uint32_t XGetAVPack()
{
    return 0;
}

void XamLoaderTerminateTitle()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XamGetExecutionId()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XamLoaderLaunchTitle()
{
    LOG_UTILITY("!!! STUB !!!");
}

// NtOpenFile is essentially a simplified NtCreateFile for opening existing files
uint32_t NtOpenFile(
    be<uint32_t>* FileHandle,
    uint32_t DesiredAccess,
    XOBJECT_ATTRIBUTES* Attributes,
    XIO_STATUS_BLOCK* IoStatusBlock,
    uint32_t ShareAccess,
    uint32_t OpenOptions)
{
    (void)ShareAccess;
    (void)OpenOptions;

    if (!FileHandle || !Attributes)
        return STATUS_INVALID_PARAMETER;

    std::string guestPath;
    if (!TryGetAnsiPath(Attributes, guestPath))
        return STATUS_INVALID_PARAMETER;

    // DesiredAccess on Xbox uses Win32-like GENERIC_READ/WRITE bits.
    std::ios::openmode mode = std::ios::binary;
    if (DesiredAccess & (GENERIC_READ | FILE_READ_DATA))
        mode |= std::ios::in;
    if (DesiredAccess & GENERIC_WRITE)
        mode |= std::ios::out;

    // Fallback: treat unknown access flags as read.
    if ((mode & (std::ios::in | std::ios::out)) == 0)
        mode |= std::ios::in;

    std::filesystem::path resolved = ResolveGuestPathBestEffort(guestPath);

    LOGF_IMPL(Utility, "NtOpenFile", "Guest: '{}' -> Resolved: '{}'", guestPath, resolved.string());

    // Check for directory
    {
        std::error_code ec;
        bool exists = std::filesystem::exists(resolved, ec);
        bool isDir = exists && std::filesystem::is_directory(resolved, ec);
        
        LOGF_IMPL(Utility, "NtOpenFile", "exists={} isDir={} ec={}", exists, isDir, ec ? ec.message() : "none");
        
        if (exists && isDir && !ec)
        {
            NtDirHandle* hDir = CreateKernelObject<NtDirHandle>();
            hDir->path = resolved;

            const uint32_t handleValue = GetKernelHandle(hDir);
            g_ntDirHandles.emplace(handleValue, hDir);

            *FileHandle = handleValue;
            LOGF_IMPL(Utility, "NtOpenFile", "Created directory handle 0x{:08X} for {}", handleValue, resolved.string());
            if (IoStatusBlock)
            {
                IoStatusBlock->Status = STATUS_SUCCESS;
                IoStatusBlock->Information = 1;
            }

            return STATUS_SUCCESS;
        }
    }

    std::fstream stream;
    stream.open(resolved, mode);

    if (!stream.is_open())
    {
        const uint32_t status = ErrnoToNtStatus(errno);
        LOGF_IMPL(Utility, "NtOpenFile", "FAILED to open '{}' -> '{}': errno={} status=0x{:08X}", 
                  guestPath, resolved.string(), errno, status);
        if (IoStatusBlock)
        {
            IoStatusBlock->Status = status;
            IoStatusBlock->Information = 0;
        }
        *FileHandle = 0;
        return status;
    }

    NtFileHandle* h = CreateKernelObject<NtFileHandle>();
    h->stream = std::move(stream);
    h->path = std::move(resolved);
    // Mark RPF files for special handling (TOC decryption)
    try {
        std::string ext = h->path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });
        if (ext == ".rpf")
            h->isRpf = true;
    } catch (...) {}

    const uint32_t handleValue = GetKernelHandle(h);
    g_ntFileHandles.emplace(handleValue, h);

    *FileHandle = handleValue;
    LOGF_IMPL(Utility, "NtOpenFile", "Opened file handle 0x{:08X} for {}", handleValue, resolved.string());
    if (IoStatusBlock)
    {
        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = 1;
    }
    return STATUS_SUCCESS;
}

void RtlInitAnsiString(XANSI_STRING* destination, char* source)
{
    const uint16_t length = source ? (uint16_t)strlen(source) : 0;
    destination->Length = length;
    destination->MaximumLength = length + 1;
    destination->Buffer = source;
}

uint32_t NtCreateFile
(
    be<uint32_t>* FileHandle,
    uint32_t DesiredAccess,
    XOBJECT_ATTRIBUTES* Attributes,
    XIO_STATUS_BLOCK* IoStatusBlock,
    uint64_t* AllocationSize,
    uint32_t FileAttributes,
    uint32_t ShareAccess,
    uint32_t CreateDisposition
)
{
    // Pump SDL events to keep window responsive
    PumpSdlEventsIfNeeded();
    
    (void)AllocationSize;
    (void)FileAttributes;
    (void)ShareAccess;

    if (!FileHandle || !Attributes)
        return STATUS_INVALID_PARAMETER;

    std::string guestPath;
    if (!TryGetAnsiPath(Attributes, guestPath))
        return STATUS_INVALID_PARAMETER;

    // DesiredAccess on Xbox uses Win32-like GENERIC_READ/WRITE bits.
    std::ios::openmode mode = std::ios::binary;
    if (DesiredAccess & (GENERIC_READ | FILE_READ_DATA))
        mode |= std::ios::in;
    if (DesiredAccess & GENERIC_WRITE)
        mode |= std::ios::out;

    // Fallback: treat unknown access flags as read.
    if ((mode & (std::ios::in | std::ios::out)) == 0)
        mode |= std::ios::in;

    std::filesystem::path resolved = ResolveGuestPathBestEffort(guestPath);

    // Debug: Log resolved path for troubleshooting
    LOGF_IMPL(Utility, "NtCreateFile", "Guest: '{}' -> Resolved: '{}' DesiredAccess=0x{:08X}", guestPath, resolved.string(), DesiredAccess);

    // Check if this is a direct .rpf file open (e.g., "game:\common.rpf")
    {
        std::string pathLower = guestPath;
        std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), ::tolower);
        if (pathLower.find(".rpf") != std::string::npos)
        {
            LOGF_IMPL(Utility, "NtCreateFile", "RPF file access detected: '{}'", guestPath);
        }
    }

    // GTA IV RPF handling: When the game tries to open a path ending with backslash
    // (like "fxl_final\" or "game:\") as if it were a file, it's trying to mount an RPF archive.
    // Since we've extracted the RPF contents, we should let the game fail gracefully here
    // so it falls back to accessing extracted files individually.
    // However, if the game opens these for directory enumeration (not file reading), we allow it.
    bool isRpfMountAttempt = !guestPath.empty() && 
                              (guestPath.back() == '\\' || guestPath.back() == '/') &&
                              (DesiredAccess & (GENERIC_READ | FILE_READ_DATA)) &&
                              !(DesiredAccess & GENERIC_WRITE);

    // Directories can't be opened with std::fstream; represent them with a dedicated handle.
    {
        std::error_code ec;
        bool exists = std::filesystem::exists(resolved, ec);
        bool isDir = exists && std::filesystem::is_directory(resolved, ec);
        LOGF_IMPL(Utility, "NtCreateFile", "exists={} isDir={} ec={}", exists, isDir, ec ? ec.message() : "none");
        
        if (exists && isDir && !ec)
        {
            // GTA IV behavior: it opens `game:\\` (and sometimes `D:\\`) as if it were a file
            // and then reads an RPF header from it as part of its packfile/disc mounting logic.
            // On our host FS `game` resolves to a directory, which makes reads fail and triggers
            // the dirty-disc loop.
            //
            // Fix: for mount-style opens on the root, map that open to real .rpf files and let
            // the title parse them using normal NtReadFile semantics.
            // For all other directories, always return a directory handle so enumeration works.
            auto isRootMountPath = [](const std::string& path) -> bool
            {
                std::string_view p(path);
                while (!p.empty() && (p.back() == '\\' || p.back() == '/'))
                    p.remove_suffix(1);

                auto ieq = [](std::string_view a, std::string_view b) -> bool
                {
                    if (a.size() != b.size())
                        return false;
                    for (size_t i = 0; i < a.size(); i++)
                    {
                        char ca = a[i];
                        char cb = b[i];
                        if (ca >= 'A' && ca <= 'Z')
                            ca = char(ca - 'A' + 'a');
                        if (cb >= 'A' && cb <= 'Z')
                            cb = char(cb - 'A' + 'a');
                        if (ca != cb)
                            return false;
                    }
                    return true;
                };

                return ieq(p, "game:") || ieq(p, "d:");
            };

            if (isRpfMountAttempt)
            {
                // GTA IV opens directories with trailing backslash as if they were RPF files.
                // For game:\ root, treat it as a directory so the game can enumerate RPF files.
                // For paths like fxl_final\, these are inside RPFs - redirect to extracted content.
                
                std::string pathLower = guestPath;
                std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), ::tolower);
                
                // Strip trailing slashes
                while (!pathLower.empty() && (pathLower.back() == '\\' || pathLower.back() == '/'))
                    pathLower.pop_back();
                
                // For game:\ root, return as directory handle so game can enumerate files
                if (pathLower == "game:" || pathLower == "d:")
                {
                    // Return directory handle for game root
                    NtDirHandle* hDir = CreateKernelObject<NtDirHandle>();
                    hDir->path = resolved;

                    const uint32_t handleValue = GetKernelHandle(hDir);
                    g_ntDirHandles.emplace(handleValue, hDir);

                    *FileHandle = handleValue;
                    LOGF_IMPL(Utility, "NtCreateFile", "game:\\ root -> directory handle 0x{:08X} for {}", handleValue, resolved.string());
                    
                    if (IoStatusBlock)
                    {
                        IoStatusBlock->Status = STATUS_SUCCESS;
                        IoStatusBlock->Information = 1;
                    }
                    return STATUS_SUCCESS;
                }
                
                // For paths like fxl_final\, these are inside common.rpf
                // Open common.rpf as a file handle so sub_829A1F00 can read from it
                if (pathLower.find("fxl_final") != std::string::npos)
                {
                    // Open common.rpf for reading
                    std::filesystem::path rpfPath = GetGamePath() / "game" / "common.rpf";
                    if (std::filesystem::exists(rpfPath))
                    {
                        std::fstream rpfStream;
                        rpfStream.open(rpfPath, std::ios::in | std::ios::binary);
                        
                        if (rpfStream.is_open())
                        {
                            NtFileHandle* hFile = CreateKernelObject<NtFileHandle>();
                            hFile->stream = std::move(rpfStream);
                            hFile->path = rpfPath;
                            hFile->isRpf = true;
                            
                            const uint32_t handleValue = GetKernelHandle(hFile);
                            g_ntFileHandles.emplace(handleValue, hFile);
                            
                            // Parse RPF header immediately
                            ParseRpfHeader(hFile);

                            *FileHandle = handleValue;
                            LOGF_IMPL(Utility, "NtCreateFile", "fxl_final -> opened common.rpf for reading (handle=0x{:08X})", 
                                      handleValue);
                            
                            if (IoStatusBlock)
                            {
                                IoStatusBlock->Status = STATUS_SUCCESS;
                                IoStatusBlock->Information = 1;
                            }
                            return STATUS_SUCCESS;
                        }
                    }
                    
                    // Fallback to extracted directory if RPF not available
                    std::filesystem::path extractedPath = GetGamePath() / "RPF DUMP" / "common" / "shaders" / "fxl_final";
                    if (std::filesystem::exists(extractedPath))
                    {
                        NtDirHandle* hDir = CreateKernelObject<NtDirHandle>();
                        hDir->path = extractedPath;

                        const uint32_t handleValue = GetKernelHandle(hDir);
                        g_ntDirHandles.emplace(handleValue, hDir);

                        *FileHandle = handleValue;
                        LOGF_IMPL(Utility, "NtCreateFile", "fxl_final -> fallback to extracted dir: {} (handle=0x{:08X})", 
                                  extractedPath.string(), handleValue);
                        
                        if (IoStatusBlock)
                        {
                            IoStatusBlock->Status = STATUS_SUCCESS;
                            IoStatusBlock->Information = 1;
                        }
                        return STATUS_SUCCESS;
                    }
                }
                
                // For other RPF mount attempts, return virtual empty RPF
                NtVirtFileHandle* hVirt = CreateKernelObject<NtVirtFileHandle>();
                hVirt->debugPath = resolved;
                hVirt->data = MakeEmptyRpfImage();

                const uint32_t handleValue = GetKernelHandle(hVirt);
                g_ntVirtFileHandles.emplace(handleValue, hVirt);

                *FileHandle = handleValue;
                LOGF_IMPL(Utility, "NtCreateFile", "RPF mount '{}' -> virtual empty RPF (handle=0x{:08X})", guestPath, handleValue);
                
                if (IoStatusBlock)
                {
                    IoStatusBlock->Status = STATUS_SUCCESS;
                    IoStatusBlock->Information = 1;
                }

                return STATUS_SUCCESS;
            }
            else
            {
                NtDirHandle* hDir = CreateKernelObject<NtDirHandle>();
                hDir->path = resolved;

                const uint32_t handleValue = GetKernelHandle(hDir);
                g_ntDirHandles.emplace(handleValue, hDir);

                *FileHandle = handleValue;
                LOGF_IMPL(Utility, "NtCreateFile", "Created directory handle 0x{:08X} for {}", handleValue, resolved.string());
            }
            if (IoStatusBlock)
            {
                IoStatusBlock->Status = STATUS_SUCCESS;
                IoStatusBlock->Information = 1;
            }

            return STATUS_SUCCESS;
        }
    }

    std::fstream stream;
    stream.open(resolved, mode);

    if (!stream.is_open())
    {
        const uint32_t status = ErrnoToNtStatus(errno);
        LOGF_IMPL(Utility, "NtCreateFile", "FAILED to open '{}' -> '{}': errno={} status=0x{:08X}", 
                  guestPath, resolved.string(), errno, status);
        if (IoStatusBlock)
        {
            IoStatusBlock->Status = status;
            IoStatusBlock->Information = 0;
        }
        *FileHandle = 0;
        return status;
    }

    // TODO: Respect CreateDisposition if the title needs create/overwrite semantics.
    (void)CreateDisposition;

    NtFileHandle* h = CreateKernelObject<NtFileHandle>();
    h->stream = std::move(stream);
    h->path = std::move(resolved);

    const uint32_t handleValue = GetKernelHandle(h);
    g_ntFileHandles.emplace(handleValue, h);

    *FileHandle = handleValue;
    if (IoStatusBlock)
    {
        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = 1; // FILE_OPENED (best-effort)
    }

    return STATUS_SUCCESS;
}

uint32_t NtClose(uint32_t handle)
{
    if (handle == GUEST_INVALID_HANDLE_VALUE)
        return 0xFFFFFFFF;

    if (IsKernelObject(handle))
    {
        // If the handle was duplicated, just decrement the duplication count. Otherwise, delete the object.
        auto it = g_handleDuplicates.find(handle);
        const bool willDestroy = (it == g_handleDuplicates.end() || it->second == 0);
        if (!willDestroy)
        {
            if (--it->second == 0)
                g_handleDuplicates.erase(it);
            return 0;
        }

        // Close NT file handles we created.
        if (auto fhIt = g_ntFileHandles.find(handle); fhIt != g_ntFileHandles.end())
        {
            if (fhIt->second)
                fhIt->second->stream.close();
            g_ntFileHandles.erase(fhIt);
        }

        if (auto dhIt = g_ntDirHandles.find(handle); dhIt != g_ntDirHandles.end())
        {
            g_ntDirHandles.erase(dhIt);
        }

        if (auto vhIt = g_ntVirtFileHandles.find(handle); vhIt != g_ntVirtFileHandles.end())
        {
            g_ntVirtFileHandles.erase(vhIt);
        }

        DestroyKernelObject(handle);

        return 0;
    }
    else
    {
        assert(false && "Unrecognized kernel object.");
        return 0xFFFFFFFF;
    }
}

void NtSetInformationFile()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t FscSetCacheElementCount()
{
    return 0;
}

uint32_t FscGetCacheElementCount()
{
    return 0;
}

uint32_t XamLoaderGetLaunchDataSize()
{
    return 0;
}

uint32_t XamLoaderGetLaunchData()
{
    return 0;
}

uint32_t XamLoaderSetLaunchData()
{
    return 0;
}

uint32_t NtWaitForSingleObjectEx(uint32_t Handle, uint32_t WaitMode, uint32_t Alertable, be<int64_t>* Timeout)
{
    if (Handle == GUEST_INVALID_HANDLE_VALUE)
        return 0xFFFFFFFF;

    uint32_t timeout = GuestTimeoutToMilliseconds(Timeout);
    // assert(timeout == 0 || timeout == INFINITE);

    if (IsKernelObject(Handle))
    {
        return GetKernelObject(Handle)->Wait(timeout);
    }
    else
    {
        assert(false && "Unrecognized handle value.");
    }

    return STATUS_TIMEOUT;
}

uint32_t NtWriteFile(
    uint32_t FileHandle,
    uint32_t /*Event*/, 
    uint32_t /*ApcRoutine*/,
    uint32_t /*ApcContext*/,
    XIO_STATUS_BLOCK* IoStatusBlock,
    const void* Buffer,
    uint32_t Length,
    be<int64_t>* ByteOffset)
{
    if (FileHandle == GUEST_INVALID_HANDLE_VALUE || !IsKernelObject(FileHandle))
    {
        LOGF_IMPL(Utility, "NtWriteFile", "INVALID handle 0x{:08X}", FileHandle);
        return STATUS_INVALID_HANDLE;
    }

    auto it = g_ntFileHandles.find(FileHandle);
    if (it == g_ntFileHandles.end() || !it->second || it->second->magic != kNtFileHandleMagic)
    {
        LOGF_IMPL(Utility, "NtWriteFile", "Not a file handle 0x{:08X}", FileHandle);
        return STATUS_INVALID_HANDLE;
    }

    NtFileHandle* hFile = it->second;
    if (!Buffer)
        return STATUS_INVALID_PARAMETER;

    if (ByteOffset != nullptr)
    {
        const int64_t offset = ByteOffset->get();
        hFile->stream.clear();
        hFile->stream.seekp(offset, std::ios::beg);
        if (hFile->stream.bad())
            return STATUS_FAIL_CHECK;
    }

    hFile->stream.write(reinterpret_cast<const char*>(Buffer), Length);
    const bool ok = !hFile->stream.bad();

    if (IoStatusBlock)
    {
        IoStatusBlock->Status = ok ? STATUS_SUCCESS : STATUS_FAIL_CHECK;
        IoStatusBlock->Information = ok ? Length : 0;
    }

    return ok ? STATUS_SUCCESS : STATUS_FAIL_CHECK;
}

void vsprintf_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t ExGetXConfigSetting(uint16_t Category, uint16_t Setting, void* Buffer, uint16_t SizeOfBuffer, be<uint32_t>* RequiredSize)
{
    uint32_t data[4]{};

    switch (Category)
    {
        // XCONFIG_SECURED_CATEGORY
        case 0x0002:
        {
            switch (Setting)
            {
                // XCONFIG_SECURED_AV_REGION
                case 0x0002:
                    data[0] = ByteSwap(0x00001000); // USA/Canada
                    break;

                default:
                    return 1;
            }
        }

        case 0x0003:
        {
            switch (Setting)
            {
                case 0x0001: // XCONFIG_USER_TIME_ZONE_BIAS
                case 0x0002: // XCONFIG_USER_TIME_ZONE_STD_NAME
                case 0x0003: // XCONFIG_USER_TIME_ZONE_DLT_NAME
                case 0x0004: // XCONFIG_USER_TIME_ZONE_STD_DATE
                case 0x0005: // XCONFIG_USER_TIME_ZONE_DLT_DATE
                case 0x0006: // XCONFIG_USER_TIME_ZONE_STD_BIAS
                case 0x0007: // XCONFIG_USER_TIME_ZONE_DLT_BIAS
                    data[0] = 0;
                    break;

                // XCONFIG_USER_LANGUAGE
                case 0x0009:
                    data[0] = ByteSwap((uint32_t)Config::Language.Value);
                    break;

                // XCONFIG_USER_VIDEO_FLAGS
                case 0x000A:
                    data[0] = ByteSwap(0x00040000);
                    break;

                // XCONFIG_USER_RETAIL_FLAGS
                case 0x000C:
                    data[0] = ByteSwap(1);
                    break;

                // XCONFIG_USER_COUNTRY
                case 0x000E:
                    data[0] = ByteSwap(103);
                    break;

                default:
                    return 1;
            }
        }
    }

    *RequiredSize = 4;
    memcpy(Buffer, data, std::min((size_t)SizeOfBuffer, sizeof(data)));

    return 0;
}

void NtQueryVirtualMemory()
{
    LOG_UTILITY("!!! STUB !!!");
}

#ifndef STATUS_INVALID_PARAMETER
#define STATUS_INVALID_PARAMETER 0xC000000D
#endif

#ifndef STATUS_NO_MEMORY
#define STATUS_NO_MEMORY 0xC0000017
#endif

void MmQueryStatistics()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t NtCreateEvent(be<uint32_t>* handle, void* objAttributes, uint32_t eventType, uint32_t initialState)
{
    *handle = GetKernelHandle(CreateKernelObject<Event>(!eventType, !!initialState));
    return 0;
}

uint32_t XexCheckExecutablePrivilege()
{
    return 0;
}

void DbgPrint()
{
    LOG_UTILITY("!!! STUB !!!");
}

void __C_specific_handler_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t RtlNtStatusToDosError(uint32_t Status)
{
    // See https://github.com/wine-mirror/wine/blob/master/dlls/ntdll/error.c#L47-L64
    if (Status == 0 || (Status & 0x20000000) != 0)
        return Status;

    if ((Status & 0xF0000000) == 0xD0000000)
        Status &= ~0x10000000;

    const uint32_t hi = (Status >> 16) & 0xFFFF;
    if (hi == 0x8007 || hi == 0xC001 || hi == 0xC007)
        return Status & 0xFFFF;

    switch (Status)
    {
    case uint32_t(STATUS_NOT_IMPLEMENTED):
        return ERROR_CALL_NOT_IMPLEMENTED;
    case uint32_t(STATUS_SEMAPHORE_LIMIT_EXCEEDED):
        return ERROR_TOO_MANY_POSTS;
    case uint32_t(STATUS_OBJECT_NAME_NOT_FOUND):
        return ERROR_FILE_NOT_FOUND;
    case uint32_t(STATUS_INVALID_PARAMETER):
        return ERROR_INVALID_PARAMETER;
    case uint32_t(STATUS_INVALID_HANDLE):
        return ERROR_INVALID_HANDLE;
    case uint32_t(STATUS_END_OF_FILE):
        return ERROR_HANDLE_EOF;
    case uint32_t(STATUS_NO_MORE_FILES):
        return ERROR_NO_MORE_FILES;
    case uint32_t(STATUS_ACCESS_DENIED):
        return ERROR_ACCESS_DENIED;
    case uint32_t(STATUS_OBJECT_NAME_INVALID):
        return ERROR_INVALID_NAME;
    case uint32_t(STATUS_OBJECT_PATH_NOT_FOUND):
        return ERROR_PATH_NOT_FOUND;
    case uint32_t(STATUS_OBJECT_NAME_COLLISION):
        return ERROR_ALREADY_EXISTS;
    default:
        LOGF_WARNING("Unimplemented NtStatus translation: {:#08x}", Status);
        return Status;
    }
}

void XexGetProcedureAddress()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XexGetModuleSection()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t RtlUnicodeToMultiByteN(char* MultiByteString, uint32_t MaxBytesInMultiByteString, be<uint32_t>* BytesInMultiByteString, const be<uint16_t>* UnicodeString, uint32_t BytesInUnicodeString)
{
    const auto reqSize = BytesInUnicodeString / sizeof(uint16_t);

    if (BytesInMultiByteString)
        *BytesInMultiByteString = reqSize;

    if (reqSize > MaxBytesInMultiByteString)
        return STATUS_FAIL_CHECK;

    for (size_t i = 0; i < reqSize; i++)
    {
        const auto c = UnicodeString[i].get();

        MultiByteString[i] = c < 256 ? c : '?';
    }

    return STATUS_SUCCESS;
}

uint32_t KeDelayExecutionThread(uint32_t WaitMode, bool Alertable, be<int64_t>* Timeout)
{
    // Pump SDL events to keep window responsive
    PumpSdlEventsIfNeeded();
    
    // We don't do async file reads.
    if (Alertable)
        return STATUS_USER_APC;

    uint32_t timeout = GuestTimeoutToMilliseconds(Timeout);

#ifdef _WIN32
    Sleep(timeout);
#else
    if (timeout == 0)
        std::this_thread::yield();
    else
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
#endif

    return STATUS_SUCCESS;
}

void ExFreePool()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t NtQueryInformationFile(
    uint32_t FileHandle,
    XIO_STATUS_BLOCK* IoStatusBlock,
    void* FileInformation,
    uint32_t Length,
    uint32_t FileInformationClass)
{
    if (FileHandle == GUEST_INVALID_HANDLE_VALUE || !IsKernelObject(FileHandle))
        return STATUS_INVALID_HANDLE;

    std::filesystem::path path;
    NtFileHandle* hFile = nullptr;
    NtVirtFileHandle* hVirt = nullptr;
    if (auto it = g_ntFileHandles.find(FileHandle); it != g_ntFileHandles.end() && it->second && it->second->magic == kNtFileHandleMagic)
    {
        hFile = it->second;
        path = hFile->path;
    }
    else if (auto it = g_ntVirtFileHandles.find(FileHandle); it != g_ntVirtFileHandles.end() && it->second && it->second->magic == kNtVirtFileHandleMagic)
    {
        hVirt = it->second;
        path = hVirt->debugPath;
    }
    else if (auto it = g_ntDirHandles.find(FileHandle); it != g_ntDirHandles.end() && it->second && it->second->magic == kNtDirHandleMagic)
    {
        path = it->second->path;
    }
    else
    {
        return STATUS_INVALID_HANDLE;
    }

    if (!IoStatusBlock || !FileInformation)
        return STATUS_INVALID_PARAMETER;

    std::error_code ec;
    bool isDir = false;
    uint64_t fileSize = 0;
    uint32_t attrs = 0;

    if (hVirt)
    {
        // In-memory file: pretend it's a normal file with known size.
        isDir = false;
        fileSize = hVirt->data.size();
        attrs = ByteSwap(FILE_ATTRIBUTE_NORMAL);
    }
    else
    {
        const bool exists = std::filesystem::exists(path, ec);
        if (!exists || ec)
        {
            IoStatusBlock->Status = STATUS_OBJECT_NAME_NOT_FOUND;
            IoStatusBlock->Information = 0;
            return STATUS_OBJECT_NAME_NOT_FOUND;
        }

        isDir = std::filesystem::is_directory(path, ec);
        if (ec)
        {
            IoStatusBlock->Status = STATUS_FAIL_CHECK;
            IoStatusBlock->Information = 0;
            return STATUS_FAIL_CHECK;
        }

        if (!isDir)
        {
            fileSize = std::filesystem::file_size(path, ec);
            if (ec)
                fileSize = 0;
        }

        attrs = GetFileAttributesBestEffort(path);
    }

    const uint64_t allocationSize = RoundUpToPage(fileSize);

    switch (FileInformationClass)
    {
    case FileBasicInformation:
    {
        if (Length < sizeof(XFILE_BASIC_INFORMATION))
            return STATUS_INFO_LENGTH_MISMATCH;

        auto* info = reinterpret_cast<XFILE_BASIC_INFORMATION*>(FileInformation);
        info->CreationTime = 0;
        info->LastAccessTime = 0;
        info->LastWriteTime = 0;
        info->ChangeTime = 0;
        info->FileAttributes = attrs;
        info->Reserved = 0;

        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = sizeof(XFILE_BASIC_INFORMATION);
        return STATUS_SUCCESS;
    }
    case FileStandardInformation:
    {
        if (Length < sizeof(XFILE_STANDARD_INFORMATION))
            return STATUS_INFO_LENGTH_MISMATCH;

        auto* info = reinterpret_cast<XFILE_STANDARD_INFORMATION*>(FileInformation);
        info->AllocationSize = static_cast<int64_t>(allocationSize);
        info->EndOfFile = static_cast<int64_t>(fileSize);
        info->NumberOfLinks = 1;
        info->DeletePending = 0;
        info->Directory = isDir ? 1 : 0;
        info->Reserved = 0;

        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = sizeof(XFILE_STANDARD_INFORMATION);
        return STATUS_SUCCESS;
    }
    case FilePositionInformation:
    {
        if (Length < sizeof(XFILE_POSITION_INFORMATION))
            return STATUS_INFO_LENGTH_MISMATCH;

        auto* info = reinterpret_cast<XFILE_POSITION_INFORMATION*>(FileInformation);
        if (hFile)
        {
            const auto pos = hFile->stream.tellg();
            if (pos < 0)
                info->CurrentByteOffset = 0;
            else
                info->CurrentByteOffset = static_cast<int64_t>(pos);
        }
        else
        {
            info->CurrentByteOffset = 0;
        }

        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = sizeof(XFILE_POSITION_INFORMATION);
        return STATUS_SUCCESS;
    }
    case FileEndOfFileInformation:
    {
        if (Length < sizeof(XFILE_END_OF_FILE_INFORMATION))
            return STATUS_INFO_LENGTH_MISMATCH;

        auto* info = reinterpret_cast<XFILE_END_OF_FILE_INFORMATION*>(FileInformation);
        info->EndOfFile = static_cast<int64_t>(fileSize);

        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = sizeof(XFILE_END_OF_FILE_INFORMATION);
        return STATUS_SUCCESS;
    }
    case FileNetworkOpenInformation:
    {
        if (Length < sizeof(XFILE_NETWORK_OPEN_INFORMATION))
            return STATUS_INFO_LENGTH_MISMATCH;

        auto* info = reinterpret_cast<XFILE_NETWORK_OPEN_INFORMATION*>(FileInformation);
        info->CreationTime = 0;
        info->LastAccessTime = 0;
        info->LastWriteTime = 0;
        info->ChangeTime = 0;
        info->AllocationSize = static_cast<int64_t>(allocationSize);
        info->EndOfFile = static_cast<int64_t>(fileSize);
        info->FileAttributes = attrs;
        info->Reserved = 0;

        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = sizeof(XFILE_NETWORK_OPEN_INFORMATION);
        return STATUS_SUCCESS;
    }
    case 26: // Xbox 360 specific - appears to query file size/info
    {
        // Xbox 360 games may use different class numbers
        // Class 26 appears to be used for file size queries
        // Return file size information in a generic format
        if (Length >= 8)
        {
            // Return file size as first 8 bytes (common pattern)
            auto* sizePtr = reinterpret_cast<int64_t*>(FileInformation);
            *sizePtr = static_cast<int64_t>(fileSize);
            if (Length > 8)
                memset(reinterpret_cast<uint8_t*>(FileInformation) + 8, 0, Length - 8);
            
            LOGF_IMPL(Utility, "NtQueryInformationFile", "Class 26 (Xbox size query) for '{}': returning size={}", 
                      path.filename().string(), fileSize);
        }
        else if (Length > 0)
        {
            memset(FileInformation, 0, Length);
        }
        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = Length;
        return STATUS_SUCCESS;
    }
    default:
        // For unhandled classes, try to return success with zeroed data
        // This helps games that query for optional information
        if (Length > 0) {
            memset(FileInformation, 0, Length);
        }
        LOGF_WARNING("NtQueryInformationFile: unhandled class {} for '{}' - returning success with zeroed data", 
                     FileInformationClass, path.filename().string());
        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = Length;
        return STATUS_SUCCESS;
    }
}

uint32_t NtQueryVolumeInformationFile(
    uint32_t FileHandle,
    XIO_STATUS_BLOCK* IoStatusBlock,
    void* FsInformation,
    uint32_t Length,
    uint32_t FsInformationClass)
{
    if (FileHandle == GUEST_INVALID_HANDLE_VALUE || !IsKernelObject(FileHandle))
        return STATUS_INVALID_HANDLE;

    // Accept both file handles and directory handles for volume information.
    std::filesystem::path path;
    if (auto it = g_ntFileHandles.find(FileHandle); it != g_ntFileHandles.end() && it->second && it->second->magic == kNtFileHandleMagic)
    {
        path = it->second->path;
    }
    else if (auto it = g_ntVirtFileHandles.find(FileHandle); it != g_ntVirtFileHandles.end() && it->second && it->second->magic == kNtVirtFileHandleMagic)
    {
        path = it->second->debugPath;
    }
    else if (auto it = g_ntDirHandles.find(FileHandle); it != g_ntDirHandles.end() && it->second && it->second->magic == kNtDirHandleMagic)
    {
        path = it->second->path;
    }
    else
    {
        return STATUS_INVALID_HANDLE;
    }

    if (!IoStatusBlock || !FsInformation)
        return STATUS_INVALID_PARAMETER;

    switch (FsInformationClass)
    {
    case FileFsDeviceInformation:
    {
        if (Length < sizeof(XFILE_FS_DEVICE_INFORMATION))
            return STATUS_INFO_LENGTH_MISMATCH;

        auto* info = reinterpret_cast<XFILE_FS_DEVICE_INFORMATION*>(FsInformation);
        info->DeviceType = 0x00000007; // FILE_DEVICE_DISK
        info->Characteristics = 0;

        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = sizeof(XFILE_FS_DEVICE_INFORMATION);
        return STATUS_SUCCESS;
    }
    case FileFsSizeInformation:
    {
        if (Length < sizeof(XFILE_FS_SIZE_INFORMATION))
            return STATUS_INFO_LENGTH_MISMATCH;

        auto* info = reinterpret_cast<XFILE_FS_SIZE_INFORMATION*>(FsInformation);
        // Very rough defaults: 512-byte sectors, 8 sectors per allocation unit (4KB).
        info->BytesPerSector = 512;
        info->SectorsPerAllocationUnit = 8;
        info->TotalAllocationUnits = 0x100000;
        info->AvailableAllocationUnits = 0x080000;

        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = sizeof(XFILE_FS_SIZE_INFORMATION);
        return STATUS_SUCCESS;
    }
    case FileFsAttributeInformation:
    {
        // Return a fixed buffer with a short filesystem name.
        constexpr uint16_t kName[] = { 'F', 'A', 'T', 'X' };
        constexpr uint32_t kNameBytes = sizeof(kName);
        constexpr uint32_t kFixedSize = sizeof(XFILE_FS_ATTRIBUTE_INFORMATION_FIXED);

        if (Length < kFixedSize)
            return STATUS_INFO_LENGTH_MISMATCH;

        auto* info = reinterpret_cast<XFILE_FS_ATTRIBUTE_INFORMATION_FIXED*>(FsInformation);
        info->FileSystemAttributes = 0;
        info->MaximumComponentNameLength = 255;
        info->FileSystemNameLength = kNameBytes;
        memset(info->FileSystemName, 0, sizeof(info->FileSystemName));
        for (size_t i = 0; i < (sizeof(kName) / sizeof(kName[0])); ++i)
            info->FileSystemName[i] = kName[i];

        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = kFixedSize;
        return STATUS_SUCCESS;
    }
    default:
        LOGF_WARNING("NtQueryVolumeInformationFile: unhandled class {}", FsInformationClass);
        IoStatusBlock->Status = STATUS_INVALID_PARAMETER;
        IoStatusBlock->Information = 0;
        return STATUS_INVALID_PARAMETER;
    }
}

uint32_t NtQueryDirectoryFile(
    uint32_t FileHandle,
    uint32_t /*Event*/,
    uint32_t /*ApcRoutine*/,
    uint32_t /*ApcContext*/,
    XIO_STATUS_BLOCK* IoStatusBlock,
    void* FileInformation,
    uint32_t Length,
    uint32_t FileInformationClass,
    uint32_t ReturnSingleEntry,
    XANSI_STRING* FileName,
    uint32_t RestartScan)
{
    LOGF_IMPL(Utility, "NtQueryDirectoryFile", "Handle=0x{:08X} Class={} RestartScan={}", FileHandle, FileInformationClass, RestartScan);
    if (FileHandle == GUEST_INVALID_HANDLE_VALUE || !IsKernelObject(FileHandle))
    {
        LOG_IMPL(Utility, "NtQueryDirectoryFile", "INVALID_HANDLE_VALUE or not kernel object");
        return STATUS_INVALID_HANDLE;
    }

    auto it = g_ntDirHandles.find(FileHandle);
    if (it == g_ntDirHandles.end() || !it->second || it->second->magic != kNtDirHandleMagic)
    {
        LOGF_IMPL(Utility, "NtQueryDirectoryFile", "Handle 0x{:08X} not in g_ntDirHandles (found={}, hasSecond={}, magic=0x{:08X})",
            FileHandle,
            it != g_ntDirHandles.end(), it != g_ntDirHandles.end() && it->second != nullptr,
            (it != g_ntDirHandles.end() && it->second) ? it->second->magic : 0);
        return STATUS_INVALID_HANDLE;
    }
    LOGF_IMPL(Utility, "NtQueryDirectoryFile", "Dir path: '{}'", it->second->path.string());

    NtDirHandle* hDir = it->second;
    if (!IoStatusBlock || !FileInformation || Length == 0)
        return STATUS_INVALID_PARAMETER;

    if (RestartScan)
        hDir->cursor = 0;

    if (!hDir->initialized)
    {
        std::error_code ec;
        for (auto iter = std::filesystem::directory_iterator(hDir->path, ec); !ec && iter != std::filesystem::directory_iterator(); ++iter)
            hDir->entries.emplace_back(*iter);

        // Stable, deterministic ordering helps with repeatability.
        std::sort(hDir->entries.begin(), hDir->entries.end(), [](const auto& a, const auto& b) {
            return a.path().filename().string() < b.path().filename().string();
        });

        hDir->initialized = true;
        LOGF_IMPL(Utility, "NtQueryDirectoryFile", "Enumerated {} files in '{}'", hDir->entries.size(), hDir->path.string());
    }

    std::string filterName;
    if (FileName && FileName->Buffer.get() && FileName->Length.get() != 0)
    {
        const char* buf = FileName->Buffer.get();
        filterName.assign(buf, buf + FileName->Length.get());
    }

    const auto writeUtf16Be = [](be<uint16_t>* out, const std::string& s) {
        for (size_t i = 0; i < s.size(); ++i)
            out[i] = static_cast<uint16_t>(static_cast<unsigned char>(s[i]));
    };

    uint8_t* out = reinterpret_cast<uint8_t*>(FileInformation);
    uint32_t remaining = Length;
    uint32_t written = 0;

    // Track the prior entry so we can fill in its NextEntryOffset.
    be<uint32_t>* prevNextEntryOffset = nullptr;
    uint32_t prevEntrySize = 0;

    const bool single = (ReturnSingleEntry != 0);

    while (hDir->cursor < hDir->entries.size())
    {
        const auto& entry = hDir->entries[hDir->cursor];
        const std::string name = entry.path().filename().string();

        if (!filterName.empty() && name != filterName)
        {
            ++hDir->cursor;
            continue;
        }

        std::error_code ec;
        const std::filesystem::path entryPath = entry.path();
        const bool isDir = std::filesystem::is_directory(entryPath, ec);
        uint64_t fileSize = 0;
        if (!isDir)
        {
            fileSize = std::filesystem::file_size(entryPath, ec);
            if (ec)
                fileSize = 0;
        }

        const uint64_t allocationSize = RoundUpToPage(fileSize);
        const uint32_t attrs = GetFileAttributesBestEffort(entryPath);

        const uint32_t nameBytes = static_cast<uint32_t>(name.size() * sizeof(uint16_t));

        uint32_t fixedSize = 0;
        switch (FileInformationClass)
        {
        case FileDirectoryInformation:
            fixedSize = sizeof(XFILE_DIRECTORY_INFORMATION_FIXED);
            break;
        case FileFullDirectoryInformation:
            fixedSize = sizeof(XFILE_FULL_DIR_INFORMATION_FIXED);
            break;
        case FileBothDirectoryInformation:
            fixedSize = sizeof(XFILE_BOTH_DIR_INFORMATION_FIXED);
            break;
        default:
            IoStatusBlock->Status = STATUS_INVALID_PARAMETER;
            IoStatusBlock->Information = 0;
            return STATUS_INVALID_PARAMETER;
        }

        const uint32_t entrySizeUnaligned = fixedSize + nameBytes;
        const uint32_t entrySize = AlignUp(entrySizeUnaligned, 8);

        if (entrySize > remaining)
        {
            if (written == 0)
            {
                IoStatusBlock->Status = STATUS_BUFFER_OVERFLOW;
                IoStatusBlock->Information = 0;
                return STATUS_BUFFER_OVERFLOW;
            }
            break;
        }

        // Update the previous entry now that we know we are emitting a new one.
        if (prevNextEntryOffset)
            *prevNextEntryOffset = prevEntrySize;

        uint8_t* entryBase = out + written;
        memset(entryBase, 0, entrySize);

        uint8_t* nameOut = nullptr;
        switch (FileInformationClass)
        {
        case FileDirectoryInformation:
        {
            auto* info = reinterpret_cast<XFILE_DIRECTORY_INFORMATION_FIXED*>(entryBase);
            info->NextEntryOffset = 0;
            info->FileIndex = 0;
            info->CreationTime = 0;
            info->LastAccessTime = 0;
            info->LastWriteTime = 0;
            info->ChangeTime = 0;
            info->EndOfFile = static_cast<int64_t>(fileSize);
            info->AllocationSize = static_cast<int64_t>(allocationSize);
            info->FileAttributes = attrs;
            info->FileNameLength = nameBytes;
            nameOut = entryBase + sizeof(XFILE_DIRECTORY_INFORMATION_FIXED);
            prevNextEntryOffset = &info->NextEntryOffset;
            break;
        }
        case FileFullDirectoryInformation:
        {
            auto* info = reinterpret_cast<XFILE_FULL_DIR_INFORMATION_FIXED*>(entryBase);
            info->NextEntryOffset = 0;
            info->FileIndex = 0;
            info->CreationTime = 0;
            info->LastAccessTime = 0;
            info->LastWriteTime = 0;
            info->ChangeTime = 0;
            info->EndOfFile = static_cast<int64_t>(fileSize);
            info->AllocationSize = static_cast<int64_t>(allocationSize);
            info->FileAttributes = attrs;
            info->FileNameLength = nameBytes;
            info->EaSize = 0;
            nameOut = entryBase + sizeof(XFILE_FULL_DIR_INFORMATION_FIXED);
            prevNextEntryOffset = &info->NextEntryOffset;
            break;
        }
        case FileBothDirectoryInformation:
        {
            auto* info = reinterpret_cast<XFILE_BOTH_DIR_INFORMATION_FIXED*>(entryBase);
            info->NextEntryOffset = 0;
            info->FileIndex = 0;
            info->CreationTime = 0;
            info->LastAccessTime = 0;
            info->LastWriteTime = 0;
            info->ChangeTime = 0;
            info->EndOfFile = static_cast<int64_t>(fileSize);
            info->AllocationSize = static_cast<int64_t>(allocationSize);
            info->FileAttributes = attrs;
            info->FileNameLength = nameBytes;
            info->EaSize = 0;
            info->ShortNameLength = 0;
            info->Reserved1 = 0;
            memset(info->ShortName, 0, sizeof(info->ShortName));
            nameOut = entryBase + sizeof(XFILE_BOTH_DIR_INFORMATION_FIXED);
            prevNextEntryOffset = &info->NextEntryOffset;
            break;
        }
        default:
            break;
        }

        if (nameOut)
            writeUtf16Be(reinterpret_cast<be<uint16_t>*>(nameOut), name);

        prevEntrySize = entrySize;
        written += entrySize;
        remaining -= entrySize;
        ++hDir->cursor;

        if (single)
            break;
    }

    if (written == 0)
    {
        IoStatusBlock->Status = STATUS_NO_MORE_FILES;
        IoStatusBlock->Information = 0;
        return STATUS_NO_MORE_FILES;
    }

    // Final entry in the buffer.
    if (prevNextEntryOffset)
        *prevNextEntryOffset = 0;

    IoStatusBlock->Status = STATUS_SUCCESS;
    IoStatusBlock->Information = written;
    return STATUS_SUCCESS;
}

void NtReadFileScatter()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t NtReadFile(
    uint32_t FileHandle,
    uint32_t /*Event*/,
    uint32_t /*ApcRoutine*/,
    uint32_t /*ApcContext*/,
    XIO_STATUS_BLOCK* IoStatusBlock,
    void* Buffer,
    uint32_t Length,
    be<int64_t>* ByteOffset)
{
    static int s_readCount = 0;
    ++s_readCount;
    
    const uint64_t offset = ByteOffset ? static_cast<uint64_t>(ByteOffset->get()) : 0ull;
    if (s_readCount <= 50 || s_readCount % 500 == 0)
    {
        LOGF_IMPL(Utility, "NtReadFile", "#{} handle=0x{:08X} len={} offset=0x{:X}", s_readCount, FileHandle, Length, offset);
    }
    
    // Pump SDL events to keep window responsive
    PumpSdlEventsIfNeeded();
    
    // Explicit check for NULL handle - return fatal error to break retry loops
    if (FileHandle == 0)
    {
        static int nullHandleCount = 0;
        ++nullHandleCount;
        if (nullHandleCount <= 3 || nullHandleCount % 1000 == 0) {
            LOGF_IMPL(Utility, "NtReadFile", "NULL handle 0x00000000 (count={})", nullHandleCount);
        }
        // Return INVALID_HANDLE to signal the caller to give up
        if (IoStatusBlock) {
            IoStatusBlock->Status = STATUS_INVALID_HANDLE;
            IoStatusBlock->Information = 0;
        }
        // Add small delay to prevent tight CPU loop
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return STATUS_INVALID_HANDLE;
    }
    
    if (FileHandle == GUEST_INVALID_HANDLE_VALUE || !IsKernelObject(FileHandle))
    {
        LOGF_IMPL(Utility, "NtReadFile", "INVALID handle 0x{:08X}", FileHandle);
        return STATUS_INVALID_HANDLE;
    }

    // Virtual (in-memory) file handles (used to satisfy RPF mount header reads).
    if (auto vit = g_ntVirtFileHandles.find(FileHandle);
        vit != g_ntVirtFileHandles.end() && vit->second && vit->second->magic == kNtVirtFileHandleMagic)
    {
        NtVirtFileHandle* hVirt = vit->second;
        if (!IoStatusBlock || !Buffer)
            return STATUS_INVALID_PARAMETER;

        const uint64_t offset = ByteOffset ? static_cast<uint64_t>(ByteOffset->get()) : 0ull;
        if (offset >= hVirt->data.size())
        {
            IoStatusBlock->Status = STATUS_END_OF_FILE;
            IoStatusBlock->Information = 0;
            return STATUS_END_OF_FILE;
        }

        const uint32_t available = static_cast<uint32_t>(hVirt->data.size() - offset);
        const uint32_t toCopy = std::min<uint32_t>(Length, available);
        memcpy(Buffer, hVirt->data.data() + offset, toCopy);

        IoStatusBlock->Status = STATUS_SUCCESS;
        IoStatusBlock->Information = toCopy;
        return STATUS_SUCCESS;
    }

    // Directories can't be read as files.
    if (auto dit = g_ntDirHandles.find(FileHandle); dit != g_ntDirHandles.end() && dit->second && dit->second->magic == kNtDirHandleMagic)
    {
        if (IoStatusBlock)
        {
            IoStatusBlock->Status = STATUS_INVALID_PARAMETER;
            IoStatusBlock->Information = 0;
        }
        return STATUS_INVALID_PARAMETER;
    }

    auto it = g_ntFileHandles.find(FileHandle);
    if (it == g_ntFileHandles.end() || !it->second || it->second->magic != kNtFileHandleMagic)
    {
        LOGF_IMPL(Utility, "NtReadFile", "Not a file handle 0x{:08X}", FileHandle);
        return STATUS_INVALID_HANDLE;
    }

    NtFileHandle* hFile = it->second;
    if (!IoStatusBlock || !Buffer)
        return STATUS_INVALID_PARAMETER;

    // Parse RPF header on first access so we can detect encrypted TOC ranges.
    if (!hFile->rpfHeaderParsed)
    {
        // If the path extension indicates .rpf, mark it as RPF
        if (!hFile->isRpf)
        {
            try {
                std::string ext = hFile->path.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });
                if (ext == ".rpf")
                    hFile->isRpf = true;
            } catch (...) {}
        }
        
        // Parse header if this is an RPF file
        if (hFile->isRpf)
            ParseRpfHeader(hFile);
    }

    const uint64_t fileOffset = ByteOffset ? static_cast<uint64_t>(ByteOffset->get()) : 0ull;
    if (ByteOffset != nullptr)
    {
        hFile->stream.clear();
        hFile->stream.seekg(fileOffset, std::ios::beg);
        if (hFile->stream.bad())
        {
            IoStatusBlock->Status = STATUS_FAIL_CHECK;
            IoStatusBlock->Information = 0;
            return STATUS_FAIL_CHECK;
        }
    }

    hFile->stream.read(reinterpret_cast<char*>(Buffer), Length);
    const uint32_t bytesRead = static_cast<uint32_t>(hFile->stream.gcount());

    if (bytesRead == 0 && Length != 0 && hFile->stream.eof())
    {
        IoStatusBlock->Status = STATUS_END_OF_FILE;
        IoStatusBlock->Information = 0;
        return STATUS_END_OF_FILE;
    }

    // If this is an RPF file with encrypted TOC, decrypt any portion we returned that overlaps the TOC.
    if (hFile->isRpf && hFile->tocEncrypted && bytesRead > 0)
    {
        const uint64_t tocStart = hFile->tocOffset;
        const uint64_t tocEnd = tocStart + hFile->tocSize;
        const uint64_t readStart = fileOffset;
        const uint64_t readEnd = fileOffset + bytesRead;
        const uint64_t overlapStart = std::max(readStart, tocStart);
        const uint64_t overlapEnd = std::min(readEnd, tocEnd);
        if (overlapEnd > overlapStart)
        {
            const uint32_t startInBuf = static_cast<uint32_t>(overlapStart - readStart);
            const uint32_t len = static_cast<uint32_t>(overlapEnd - overlapStart);
            // Pass TOC-relative offset for proper AES block alignment
            const uint64_t tocRelativeOffset = overlapStart - tocStart;
            DecryptRpfBufferInPlace(reinterpret_cast<uint8_t*>(Buffer) + startInBuf, len, tocRelativeOffset);
        }
    }

    const bool ok = !hFile->stream.bad();
    IoStatusBlock->Status = ok ? STATUS_SUCCESS : STATUS_FAIL_CHECK;
    IoStatusBlock->Information = ok ? bytesRead : 0;
    return ok ? STATUS_SUCCESS : STATUS_FAIL_CHECK;
}

uint32_t NtDuplicateObject(uint32_t SourceHandle, be<uint32_t>* TargetHandle, uint32_t Options)
{
    if (SourceHandle == GUEST_INVALID_HANDLE_VALUE)
        return 0xFFFFFFFF;

    if (IsKernelObject(SourceHandle))
    {
        // Increment handle duplicate count.
        const auto& it = g_handleDuplicates.find(SourceHandle);
        if (it != g_handleDuplicates.end())
            ++it->second;
        else
            g_handleDuplicates[SourceHandle] = 1;

        *TargetHandle = SourceHandle;
        return 0;
    }
    else
    {
        assert(false && "Unrecognized kernel object.");
        return 0xFFFFFFFF;
    }
}

static std::unordered_map<uint32_t, uint32_t> g_ntVirtualMemoryAllocs;

// NT-style virtual memory APIs used by some titles (e.g., GTA IV).
// Backed by the existing guest heap allocator since we map a full 4GB guest address space.
// This is intentionally minimal: it supports the common "BaseAddress == NULL" allocation path.
uint32_t NtAllocateVirtualMemory(
    be<uint32_t>* BaseAddress,
    be<uint32_t>* RegionSize,
    uint32_t AllocationType,
    uint32_t Protect,
    uint32_t ZeroBits)
{
    (void)AllocationType;
    (void)Protect;
    (void)ZeroBits;

    if (!BaseAddress || !RegionSize)
        return STATUS_INVALID_PARAMETER;

    uint32_t size = RegionSize->get();
    if (size == 0)
        return STATUS_INVALID_PARAMETER;

    // Round up to page size. X360 is page-based, callers often expect at least 4KB granularity.
    constexpr uint32_t kPageSize = 0x1000;
    size = (size + (kPageSize - 1)) & ~(kPageSize - 1);

    uint32_t requested = BaseAddress->get();
    if (requested != 0)
    {
        // We currently don't manage fixed-address reservations/commits.
        // The full guest address space is already mapped, so accept the request if it is in-range.
        if (requested >= PPC_MEMORY_SIZE)
            return STATUS_INVALID_PARAMETER;

        *RegionSize = size;
        return STATUS_SUCCESS;
    }

    void* ptr = g_userHeap.AllocPhysical(size, kPageSize);
    if (!ptr)
        return STATUS_NO_MEMORY;

    const uint32_t base = g_memory.MapVirtual(ptr);
    g_ntVirtualMemoryAllocs[base] = size;

    *BaseAddress = base;
    *RegionSize = size;
    return STATUS_SUCCESS;
}

uint32_t NtFreeVirtualMemory(
    be<uint32_t>* BaseAddress,
    be<uint32_t>* RegionSize,
    uint32_t FreeType,
    uint32_t /*Unknown*/)
{
    (void)FreeType;

    if (!BaseAddress)
        return STATUS_INVALID_PARAMETER;

    const uint32_t addr = BaseAddress->get();
    if (addr == 0)
        return STATUS_SUCCESS;

    // If MEM_RELEASE is specified, RegionSize must be 0 on Win32.
    // Some titles may pass non-zero; we ignore and free anyway.
    if (RegionSize)
        *RegionSize = 0;

    const auto it = g_ntVirtualMemoryAllocs.find(addr);
    if (it != g_ntVirtualMemoryAllocs.end())
    {
        g_userHeap.Free(g_memory.Translate(addr));
        g_ntVirtualMemoryAllocs.erase(it);
    }
    *BaseAddress = 0;
    return STATUS_SUCCESS;
}

void ObDereferenceObject(void* object)
{
    // Reference counting is not fully implemented.
    // Objects are managed by the kernel object system.
    // This is a no-op for now.
    (void)object;
}

void KeSetBasePriorityThread(GuestThreadHandle* hThread, int priority)
{
#ifdef _WIN32
    if (priority == 16)
    {
        priority = 15;
    }
    else if (priority == -16)
    {
        priority = -15;
    }

    SetThreadPriority(hThread == GetKernelObject(CURRENT_THREAD_HANDLE) ? GetCurrentThread() : hThread->thread.native_handle(), priority);
#endif
}

uint32_t ObReferenceObjectByHandle(uint32_t handle, uint32_t objectType, be<uint32_t>* object)
{
    *object = handle;
    return 0;
}

void KeQueryBasePriorityThread()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t NtSuspendThread(GuestThreadHandle* hThread, uint32_t* suspendCount)
{
    assert(hThread != GetKernelObject(CURRENT_THREAD_HANDLE) && hThread->GetThreadId() == GuestThread::GetCurrentThreadId());

    hThread->suspended = true;
    hThread->suspended.wait(true);

    return S_OK;
}

uint32_t KeSetAffinityThread(uint32_t Thread, uint32_t Affinity, be<uint32_t>* lpPreviousAffinity)
{
    if (lpPreviousAffinity)
        *lpPreviousAffinity = 2;

    return 0;
}

void RtlLeaveCriticalSection(XRTL_CRITICAL_SECTION* cs)
{
    // printf("RtlLeaveCriticalSection");
    cs->RecursionCount = cs->RecursionCount.get() - 1;

    if (cs->RecursionCount.get() != 0)
        return;

    std::atomic_ref owningThread(cs->OwningThread);
    owningThread.store(0);
    owningThread.notify_one();
}

void RtlEnterCriticalSection(XRTL_CRITICAL_SECTION* cs)
{
    uint32_t thisThread = g_ppcContext->r13.u32;
    // printf("RtlEnterCriticalSection %x %x %x %x\n", thisThread, cs->OwningThread, cs->LockCount, cs->RecursionCount);
    assert(thisThread != NULL);

    std::atomic_ref owningThread(cs->OwningThread);

    while (true) 
    {
        uint32_t previousOwner = 0;

        if (owningThread.compare_exchange_weak(previousOwner, thisThread) || previousOwner == thisThread)
        {
            cs->RecursionCount = cs->RecursionCount.get() + 1;
            return;
        }

        // printf("wait start %x\n", cs);
        owningThread.wait(previousOwner);
        // printf("wait end\n");
    }
}

void RtlImageXexHeaderField()
{
    LOG_UTILITY("!!! STUB !!!");
}

void HalReturnToFirmware()
{
    LOG_UTILITY("!!! STUB !!!");
}

void RtlFillMemoryUlong()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeBugCheckEx()
{
    __builtin_debugtrap();
}

uint32_t KeGetCurrentProcessType()
{
    return 1;
}

void RtlCompareMemoryUlong()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t RtlInitializeCriticalSection(XRTL_CRITICAL_SECTION* cs)
{
    // printf("RtlInitializeCriticalSection %x\n", cs);
    cs->Header.Absolute = 0;
    cs->LockCount = -1;
    cs->RecursionCount = 0;
    cs->OwningThread = 0;

    return 0;
}

void RtlRaiseException_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KfReleaseSpinLock(uint32_t* spinLock)
{
    std::atomic_ref spinLockRef(*spinLock);
    spinLockRef = 0;
}

void KfAcquireSpinLock(uint32_t* spinLock)
{
    std::atomic_ref spinLockRef(*spinLock);

    while (true)
    {
        uint32_t expected = 0;
        if (spinLockRef.compare_exchange_weak(expected, g_ppcContext->r13.u32))
            break;

        std::this_thread::yield();
    }
}

uint64_t KeQueryPerformanceFrequency()
{
    return 49875000;
}

void MmFreePhysicalMemory(uint32_t type, uint32_t guestAddress)
{
    if (guestAddress != NULL)
        g_userHeap.Free(g_memory.Translate(guestAddress));
}

bool VdPersistDisplay(uint32_t a1, uint32_t* a2)
{
    *a2 = NULL;
    return false;
}

void VdSwap()
{
    // VdSwap is called by the Xbox 360 GPU to present frames
    static uint32_t s_frameCount = 0;
    if (s_frameCount < 10 || (s_frameCount % 60 == 0 && s_frameCount < 600))
    {
        LOGF_UTILITY("VdSwap frame {} - presenting!", s_frameCount);
    }
    ++s_frameCount;
    
    // Pump SDL events to keep the window responsive
    // IMPORTANT: On macOS, this MUST happen on the main thread
    if (IsMainThread())
    {
        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        GameWindow::Update();
    }
    
    // Call our host rendering system to present the frame
    Video::Present();
}

void VdGetSystemCommandBuffer()
{
    static std::unordered_set<uint32_t> s_seenCallers;
    const uint32_t lr = g_ppcContext ? static_cast<uint32_t>(g_ppcContext->lr) : 0;
    if (lr != 0 && s_seenCallers.insert(lr).second)
        LOGF_UTILITY("!!! STUB !!! caller_lr=0x{:08X}", lr);
    else
        LOG_UTILITY("!!! STUB !!!");
}

void KeReleaseSpinLockFromRaisedIrql(uint32_t* spinLock)
{
    std::atomic_ref spinLockRef(*spinLock);
    spinLockRef = 0;
}

void KeAcquireSpinLockAtRaisedIrql(uint32_t* spinLock)
{
    std::atomic_ref spinLockRef(*spinLock);

    while (true)
    {
        uint32_t expected = 0;
        if (spinLockRef.compare_exchange_weak(expected, g_ppcContext->r13.u32))
            break;

        std::this_thread::yield();
    }
}

uint32_t KiApcNormalRoutineNop()
{
    return 0;
}

void VdEnableRingBufferRPtrWriteBack(uint32_t writebackAddr, uint32_t blockSizeLog2)
{
    // r3 = writeback address (physical)
    // r4 = log2(block size), typically 6
    g_gpuRingBuffer.readPtrWritebackAddr = writebackAddr;
    g_gpuRingBuffer.blockSize = 1u << blockSizeLog2;
    g_gpuRingBuffer.writebackEnabled = true;
    
    LOGF_UTILITY("writebackAddr=0x{:08X} blockSizeLog2={} blockSize={}", 
                 writebackAddr, blockSizeLog2, g_gpuRingBuffer.blockSize);
    
    // Immediately write 0 to the writeback address to indicate GPU is caught up
    if (writebackAddr != 0)
    {
        uint32_t* hostPtr = reinterpret_cast<uint32_t*>(g_memory.Translate(writebackAddr));
        if (hostPtr)
        {
            *hostPtr = 0;  // GPU read pointer = 0 (caught up with write pointer initially)
            LOGF_UTILITY("Initialized writeback at 0x{:08X} to 0", writebackAddr);
        }
    }
}

void VdInitializeRingBuffer(uint32_t physAddr, uint32_t sizeLog2)
{
    // r3 = physical address of ring buffer (from MmGetPhysicalAddress)
    // r4 = log2(size)
    g_gpuRingBuffer.ringBufferBase = physAddr;
    g_gpuRingBuffer.ringBufferSize = 1u << sizeLog2;
    g_gpuRingBuffer.initialized = true;
    
    LOGF_UTILITY("ringBufferBase=0x{:08X} sizeLog2={} size={}", 
                 physAddr, sizeLog2, g_gpuRingBuffer.ringBufferSize);
}

uint32_t MmGetPhysicalAddress(uint32_t address)
{
    LOGF_UTILITY("0x{:x}", address);
    return address;
}

void VdSetSystemCommandBufferGpuIdentifierAddress()
{
    LOG_UTILITY("!!! STUB !!!");
}

void _vsnprintf_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

void sprintf_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

int32_t ExRegisterTitleTerminateNotification(uint32_t* reg, uint32_t create)
{
    LOG_UTILITY("!!! STUB !!!");
    return 0;
}

void VdShutdownEngines()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdQueryVideoMode(XVIDEO_MODE* vm)
{
    memset(vm, 0, sizeof(XVIDEO_MODE));
    vm->DisplayWidth = 1280;
    vm->DisplayHeight = 720;
    vm->IsInterlaced = false;
    vm->IsWidescreen = true;
    vm->IsHighDefinition = true;
    vm->RefreshRate = 0x42700000;
    vm->VideoStandard = 1;
    vm->Unknown4A = 0x4A;
    vm->Unknown01 = 0x01;
}

void VdGetCurrentDisplayInformation()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdSetDisplayMode()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdSetGraphicsInterruptCallback(uint32_t callback, uint32_t userData)
{
    // r3 = callback function pointer
    // r4 = user data passed to callback
    g_gpuRingBuffer.interruptCallback = callback;
    g_gpuRingBuffer.interruptUserData = userData;
    
    LOGF_UTILITY("callback=0x{:08X} userData=0x{:08X}", callback, userData);
}

uint32_t VdInitializeEngines()
{
    g_gpuRingBuffer.enginesInitialized = true;
    LOG_UTILITY("enginesInitialized = true");
    return 1;
}

uint32_t VdIsHSIOTrainingSucceeded()
{
    // Return 1 to indicate HSIO training succeeded - this unblocks GPU init polling
    return 1;
}

void VdGetCurrentDisplayGamma()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdQueryVideoFlags()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdCallGraphicsNotificationRoutines()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdInitializeScalerCommandBuffer()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeLeaveCriticalRegion()
{
    // Critical region management for APC protection.
    // Since APCs are not fully implemented, this is a no-op.
}

uint32_t VdRetrainEDRAM()
{
    // Return 0 to indicate EDRAM retraining succeeded
    return 0;
}

uint32_t VdRetrainEDRAMWorker(uint32_t unk0)
{
    // This is called by the GPU init polling loop
    // Must set persistent state, not just return success
    g_gpuRingBuffer.edramTrainingComplete = true;
    
    static int s_callCount = 0;
    if (++s_callCount <= 3)
    {
        LOGF_UTILITY("unk0=0x{:08X} - edramTrainingComplete = true (call #{})", unk0, s_callCount);
    }
    return 0;  // Success
}

void KeEnterCriticalRegion()
{
    // Critical region management for APC protection.
    // Since APCs are not fully implemented, this is a no-op.
}

uint32_t MmAllocatePhysicalMemoryEx
(
    uint32_t flags,
    uint32_t size,
    uint32_t protect,
    uint32_t minAddress,
    uint32_t maxAddress,
    uint32_t alignment
)
{
    static std::unordered_set<uint32_t> s_seenCallers;
    const uint32_t lr = g_ppcContext ? static_cast<uint32_t>(g_ppcContext->lr) : 0;
    if (lr != 0 && s_seenCallers.insert(lr).second)
    {
        LOGF_UTILITY(
            "caller_lr=0x{:08X} flags=0x{:X} size=0x{:X} prot=0x{:X} min=0x{:08X} max=0x{:08X} align=0x{:X}",
            lr, flags, size, protect, minAddress, maxAddress, alignment);
    }
    void* host = g_userHeap.AllocPhysical(size, alignment);
    if (host == nullptr)
        return 0;

    return g_memory.MapVirtual(host);
}

void ObDeleteSymbolicLink()
{
    LOG_UTILITY("!!! STUB !!!");
}

void ObCreateSymbolicLink()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t MmQueryAddressProtect(uint32_t guestAddress)
{
    return PAGE_READWRITE;
}

void VdEnableDisableClockGating()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeBugCheck()
{
    __builtin_debugtrap();
}

void KeLockL2()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeUnlockL2()
{
    LOG_UTILITY("!!! STUB !!!");
}

bool KeSetEvent(XKEVENT* pEvent, uint32_t Increment, bool Wait)
{
    bool result = QueryKernelObject<Event>(*pEvent)->Set();

    ++g_keSetEventGeneration;
    g_keSetEventGeneration.notify_all();

    return result;
}

bool KeResetEvent(XKEVENT* pEvent)
{
    return QueryKernelObject<Event>(*pEvent)->Reset();
}

// Track recent async reads for correlation with waits
static std::atomic<uint32_t> g_lastAsyncInfo{0};
static std::atomic<int> g_asyncReadCount{0};

uint32_t KeWaitForSingleObject(XDISPATCHER_HEADER* Object, uint32_t WaitReason, uint32_t WaitMode, bool Alertable, be<int64_t>* Timeout)
{
    static int s_waitCount = 0;
    static int s_infiniteWaitCount = 0;
    ++s_waitCount;
    
    const uint32_t timeout = GuestTimeoutToMilliseconds(Timeout);
    const uint32_t caller = g_ppcContext ? static_cast<uint32_t>(g_ppcContext->lr) : 0;
    const uint32_t objAddr = Object ? static_cast<uint32_t>(reinterpret_cast<uintptr_t>(Object) & 0xFFFFFFFF) : 0;
    
    // Log ALL waits for async IO debugging - exclude GPU poll thread (0x829DDD48)
    static int s_asyncWaitLogCount = 0;
    if (g_asyncReadCount > 0 && caller != 0x829DDD48 && s_asyncWaitLogCount < 200)
    {
        ++s_asyncWaitLogCount;
        LOGF_IMPL(Utility, "KeWaitForSingleObject", 
                  "ASYNC_WAIT #{} type={} obj=0x{:08X} caller=0x{:08X} timeout={} lastAsync=0x{:08X}",
                  s_asyncWaitLogCount, Object ? Object->Type : -1, objAddr, caller, timeout, g_lastAsyncInfo.load());
    }
    
    // Log INFINITE waits more aggressively as they can block forever
    if (timeout == INFINITE)
    {
        ++s_infiniteWaitCount;
        if (s_infiniteWaitCount <= 20 || s_infiniteWaitCount % 100 == 0)
        {
            LOGF_IMPL(Utility, "KeWaitForSingleObject", "INFINITE WAIT #{} type={} obj=0x{:08X} caller=0x{:08X}", 
                      s_infiniteWaitCount, Object ? Object->Type : -1, objAddr, caller);
        }
        
        // GPU init deadlock prevention: known GPU bootstrap callers that wait forever
        // These callers are waiting for GPU readiness signals that never come from stubs.
        // We fake GPU consumption by updating the ring buffer read pointer writeback.
        static const std::unordered_set<uint32_t> s_gpuInitCallers = {
            0x829DDD48,  // VdRetrainEDRAM / GPU init wait
            0x829D8AA8,  // VdInitializeRingBuffer caller
        };
        
        if (s_gpuInitCallers.count(caller) != 0)
        {
            static int s_gpuSignalCount = 0;
            ++s_gpuSignalCount;
            
            // Log video state flags periodically for debugging
            if (s_gpuSignalCount <= 5 || s_gpuSignalCount % 1000 == 0)
            {
                LOGF_IMPL(Utility, "KeWaitForSingleObject", 
                          "GPU poll #{}: engines={} edram={} interrupt={}", 
                          s_gpuSignalCount,
                          g_gpuRingBuffer.enginesInitialized ? 1 : 0,
                          g_gpuRingBuffer.edramTrainingComplete ? 1 : 0,
                          g_gpuRingBuffer.interruptSeen ? 1 : 0);
            }
            
            // Short-circuit: if all video state flags are true, GPU init is complete
            // The game's polling loop checks memory-backed flags that we can't easily update.
            // Instead, we block this thread with a real wait to stop the spinning.
            // This allows other threads to progress while the GPU poll thread sleeps.
            if (g_gpuRingBuffer.enginesInitialized &&
                g_gpuRingBuffer.edramTrainingComplete &&
                g_gpuRingBuffer.interruptSeen)
            {
                static bool s_loggedOnce = false;
                if (!s_loggedOnce)
                {
                    s_loggedOnce = true;
                    LOGF_IMPL(Utility, "KeWaitForSingleObject", 
                              "GPU init complete - all flags true, parking poll thread (count={})", s_gpuSignalCount);
                }
                // Park this thread - sleep for a long time to stop spinning
                // The game's GPU poll thread will be effectively disabled
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                return STATUS_SUCCESS;
            }
            
            // Fake GPU consumption: set RP to ring buffer size
            if (g_gpuRingBuffer.writebackEnabled && g_gpuRingBuffer.readPtrWritebackAddr != 0)
            {
                uint32_t* writebackPtr = reinterpret_cast<uint32_t*>(
                    g_memory.Translate(g_gpuRingBuffer.readPtrWritebackAddr));
                if (writebackPtr)
                {
                    uint32_t fakeRP = g_gpuRingBuffer.ringBufferSize;
                    *writebackPtr = ByteSwap(fakeRP);
                }
            }
            
            // Signal the event
            if (Object->Type == 0 || Object->Type == 1)
            {
                QueryKernelObject<Event>(*Object)->Set();
            }
            
            // Fire graphics interrupt callback once and set interruptSeen flag
            if (!g_gpuRingBuffer.interruptFired && g_gpuRingBuffer.interruptCallback != 0)
            {
                g_gpuRingBuffer.interruptFired = true;
                g_gpuRingBuffer.interruptSeen = true;
                LOGF_IMPL(Utility, "KeWaitForSingleObject", 
                          "Firing graphics interrupt callback 0x{:08X} - interruptSeen = true", 
                          g_gpuRingBuffer.interruptCallback);
            }
            
            // After many iterations, add a small delay to prevent CPU spinning
            if (s_gpuSignalCount > 1000)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            
            return STATUS_SUCCESS;
        }
        
    }

    switch (Object->Type)
    {
        case 0:
        case 1:
            QueryKernelObject<Event>(*Object)->Wait(timeout);
            break;

        case 5:
            QueryKernelObject<Semaphore>(*Object)->Wait(timeout);
            break;

        default:
            LOGF_IMPL(Utility, "KeWaitForSingleObject", "Unrecognized kernel object type: {}", Object->Type);
            return STATUS_TIMEOUT;
    }

    return STATUS_SUCCESS;
}

static std::vector<size_t> g_tlsFreeIndices;
static size_t g_tlsNextIndex = 0;
static Mutex g_tlsAllocationMutex;

static uint32_t& KeTlsGetValueRef(size_t index)
{
    // Having this a global thread_local variable
    // for some reason crashes on boot in debug builds.
    thread_local std::vector<uint32_t> s_tlsValues;

    if (s_tlsValues.size() <= index)
    {
        s_tlsValues.resize(index + 1, 0);
    }

    return s_tlsValues[index];
}

uint32_t KeTlsGetValue(uint32_t dwTlsIndex)
{
    return KeTlsGetValueRef(dwTlsIndex);
}

uint32_t KeTlsSetValue(uint32_t dwTlsIndex, uint32_t lpTlsValue)
{
    KeTlsGetValueRef(dwTlsIndex) = lpTlsValue;
    return TRUE;
}

uint32_t KeTlsAlloc()
{
    std::lock_guard<Mutex> lock(g_tlsAllocationMutex);
    if (!g_tlsFreeIndices.empty())
    {
        size_t index = g_tlsFreeIndices.back();
        g_tlsFreeIndices.pop_back();
        return index;
    }

    return g_tlsNextIndex++;
}

uint32_t KeTlsFree(uint32_t dwTlsIndex)
{
    std::lock_guard<Mutex> lock(g_tlsAllocationMutex);
    g_tlsFreeIndices.push_back(dwTlsIndex);
    return TRUE;
}

uint32_t XMsgInProcessCall(uint32_t app, uint32_t message, be<uint32_t>* param1, be<uint32_t>* param2)
{
    if (message == 0x7001B)
    {
        uint32_t* ptr = (uint32_t*)g_memory.Translate(param1[1]);
        ptr[0] = 0;
        ptr[1] = 0;
    }

    return 0;
}

void XamUserReadProfileSettings
(
    uint32_t titleId,
    uint32_t userIndex,
    uint32_t xuidCount,
    uint64_t* xuids,
    uint32_t settingCount,
    uint32_t* settingIds,
    be<uint32_t>* bufferSize,
    void* buffer,
    void* overlapped
)
{
    if (buffer != nullptr)
    {
        memset(buffer, 0, *bufferSize);
    }
    else
    {
        *bufferSize = 4;
    }
}

void NetDll_WSAStartup()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_WSACleanup()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_socket()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_closesocket()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_setsockopt()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_bind()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_connect()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_listen()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_accept()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_select()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_recv()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_send()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_inet_addr()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll___WSAFDIsSet()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XMsgStartIORequestEx()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XexGetModuleHandle()
{
    LOG_UTILITY("!!! STUB !!!");
}

bool RtlTryEnterCriticalSection(XRTL_CRITICAL_SECTION* cs)
{
    // printf("RtlTryEnterCriticalSection\n");
    uint32_t thisThread = g_ppcContext->r13.u32;
    assert(thisThread != NULL);

    std::atomic_ref owningThread(cs->OwningThread);

    uint32_t previousOwner = 0;

    if (owningThread.compare_exchange_weak(previousOwner, thisThread) || previousOwner == thisThread)
    {
        cs->RecursionCount = cs->RecursionCount.get() + 1;
        return true;
    }

    return false;
}

void RtlInitializeCriticalSectionAndSpinCount(XRTL_CRITICAL_SECTION* cs, uint32_t spinCount)
{
    // printf("RtlInitializeCriticalSectionAndSpinCount\n");
    cs->Header.Absolute = (spinCount + 255) >> 8;
    cs->LockCount = -1;
    cs->RecursionCount = 0;
    cs->OwningThread = 0;
}

void _vswprintf_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

void _vscwprintf_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

void _swprintf_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

void _snwprintf_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XeCryptBnQwBeSigVerify()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XeKeysGetKey()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XeCryptRotSumSha()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XeCryptSha()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeEnableFpuExceptions()
{
    LOG_UTILITY("!!! STUB !!!");
}

void RtlUnwind_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

void RtlCaptureContext_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t NtQueryFullAttributesFile(XOBJECT_ATTRIBUTES* ObjectAttributes, XFILE_NETWORK_OPEN_INFORMATION* FileInformation)
{
    if (!ObjectAttributes || !FileInformation)
        return STATUS_INVALID_PARAMETER;

    std::string guestPath;
    if (!TryGetAnsiPath(ObjectAttributes, guestPath))
        return STATUS_INVALID_PARAMETER;

    const std::filesystem::path resolved = ResolveGuestPathBestEffort(guestPath);

    std::error_code ec;
    const bool exists = std::filesystem::exists(resolved, ec);
    if (!exists || ec)
        return STATUS_OBJECT_NAME_NOT_FOUND;

    const bool isDir = std::filesystem::is_directory(resolved, ec);
    if (ec)
        return STATUS_FAIL_CHECK;

    uint64_t fileSize = 0;
    if (!isDir)
    {
        fileSize = std::filesystem::file_size(resolved, ec);
        if (ec)
            fileSize = 0;
    }

    const uint64_t allocationSize = RoundUpToPage(fileSize);
    const uint32_t attrs = GetFileAttributesBestEffort(resolved);

    FileInformation->CreationTime = 0;
    FileInformation->LastAccessTime = 0;
    FileInformation->LastWriteTime = 0;
    FileInformation->ChangeTime = 0;
    FileInformation->AllocationSize = static_cast<int64_t>(allocationSize);
    FileInformation->EndOfFile = static_cast<int64_t>(fileSize);
    FileInformation->FileAttributes = attrs;
    FileInformation->Reserved = 0;

    return STATUS_SUCCESS;
}

uint32_t RtlMultiByteToUnicodeN(be<uint16_t>* UnicodeString, uint32_t MaxBytesInUnicodeString, be<uint32_t>* BytesInUnicodeString, const char* MultiByteString, uint32_t BytesInMultiByteString)
{
    uint32_t length = std::min(MaxBytesInUnicodeString / 2, BytesInMultiByteString);

    for (size_t i = 0; i < length; i++)
        UnicodeString[i] = MultiByteString[i];

    if (BytesInUnicodeString != nullptr)
        *BytesInUnicodeString = length * 2;

    return STATUS_SUCCESS;
}

void DbgBreakPoint()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t MmQueryAllocationSize(uint32_t guestAddress)
{
    if (guestAddress == 0)
        return 0;
    
    return (uint32_t)g_userHeap.Size(g_memory.Translate(guestAddress));
}

uint32_t NtClearEvent(Event* handle, uint32_t* previousState)
{
    handle->Reset();
    return 0;
}

uint32_t NtResumeThread(GuestThreadHandle* hThread, uint32_t* suspendCount)
{
    assert(hThread != GetKernelObject(CURRENT_THREAD_HANDLE));

    hThread->suspended = false;
    hThread->suspended.notify_all();

    return S_OK;
}

uint32_t NtSetEvent(Event* handle, uint32_t* previousState)
{
    handle->Set();
    return 0;
}

uint32_t NtCreateSemaphore(be<uint32_t>* Handle, XOBJECT_ATTRIBUTES* ObjectAttributes, uint32_t InitialCount, uint32_t MaximumCount)
{
    *Handle = GetKernelHandle(CreateKernelObject<Semaphore>(InitialCount, MaximumCount));
    return STATUS_SUCCESS;
}

uint32_t NtReleaseSemaphore(Semaphore* Handle, uint32_t ReleaseCount, int32_t* PreviousCount)
{
    // the game releases semaphore with 1 maximum number of releases more than once
    if (Handle->count + ReleaseCount > Handle->maximumCount)
        return STATUS_SEMAPHORE_LIMIT_EXCEEDED;

    uint32_t previousCount;
    Handle->Release(ReleaseCount, &previousCount);

    if (PreviousCount != nullptr)
        *PreviousCount = ByteSwap(previousCount);

    return STATUS_SUCCESS;
}

void NtWaitForMultipleObjectsEx()
{
    LOG_UTILITY("!!! STUB !!!");
}

void RtlCompareStringN()
{
    LOG_UTILITY("!!! STUB !!!");
}

void _snprintf_x()
{
    LOG_UTILITY("!!! STUB !!!");
}

void StfsControlDevice()
{
    LOG_UTILITY("!!! STUB !!!");
}

void StfsCreateDevice()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NtFlushBuffersFile()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeQuerySystemTime(be<uint64_t>* time)
{
    // Pump SDL events periodically to prevent window from becoming unresponsive
    PumpSdlEventsIfNeeded();
    
    constexpr int64_t FILETIME_EPOCH_DIFFERENCE = 116444736000000000LL;

    auto now = std::chrono::system_clock::now();
    auto timeSinceEpoch = now.time_since_epoch();

    int64_t currentTime100ns = std::chrono::duration_cast<std::chrono::duration<int64_t, std::ratio<1, 10000000>>>(timeSinceEpoch).count();
    currentTime100ns += FILETIME_EPOCH_DIFFERENCE;

    *time = currentTime100ns;
}

struct TIME_FIELDS {
    be<uint16_t> Year;
    be<uint16_t> Month;
    be<uint16_t> Day;
    be<uint16_t> Hour;
    be<uint16_t> Minute;
    be<uint16_t> Second;
    be<uint16_t> Milliseconds;
    be<uint16_t> Weekday;
};

void RtlTimeToTimeFields(const be<uint64_t>* time, TIME_FIELDS* timeFields)
{
    constexpr uint64_t TICKS_PER_MILLISECOND = 10000;
    constexpr uint64_t TICKS_PER_SECOND = 10000000;
    constexpr uint64_t TICKS_PER_MINUTE = 600000000;
    constexpr uint64_t TICKS_PER_HOUR = 36000000000;
    constexpr uint64_t TICKS_PER_DAY = 864000000000;

    static const int DaysInMonth[2][12] = {
            {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, // Non-leap
            {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}  // Leap
    };

    // Calculate total days since January 1, 1601
    uint64_t days = *time / TICKS_PER_DAY;
    uint64_t remainingTicks = *time % TICKS_PER_DAY;

    timeFields->Hour = static_cast<uint16_t>(remainingTicks / TICKS_PER_HOUR);
    remainingTicks %= TICKS_PER_HOUR;

    timeFields->Minute = static_cast<uint16_t>(remainingTicks / TICKS_PER_MINUTE);
    remainingTicks %= TICKS_PER_MINUTE;

    timeFields->Second = static_cast<uint16_t>(remainingTicks / TICKS_PER_SECOND);
    remainingTicks %= TICKS_PER_SECOND;

    timeFields->Milliseconds = static_cast<uint16_t>(remainingTicks / TICKS_PER_MILLISECOND);

    // Calculate day of week (January 1, 1601 was a Monday = 1)
    timeFields->Weekday = static_cast<uint16_t>((days + 1) % 7);

    // Calculate year
    uint32_t year = 1601;

    // Each 400-year cycle has 146097 days
    uint32_t cycles400 = static_cast<uint32_t>(days / 146097);
    days %= 146097;
    year += cycles400 * 400;

    // Handle 100-year cycles (24 leap years + 76 normal years = 36524 days)
    // Except the 4th century which has 36525 days
    uint32_t cycles100 = static_cast<uint32_t>(days / 36524);
    if (cycles100 == 4) cycles100 = 3; // Last day of 400-year cycle
    days -= cycles100 * 36524;
    year += cycles100 * 100;

    // Handle 4-year cycles (1 leap year + 3 normal years = 1461 days)
    uint32_t cycles4 = static_cast<uint32_t>(days / 1461);
    days %= 1461;
    year += cycles4 * 4;

    // Handle individual years within 4-year cycle
    uint32_t yearInCycle = static_cast<uint32_t>(days / 365);
    if (yearInCycle == 4) yearInCycle = 3; // Last day of leap cycle
    days -= yearInCycle * 365;
    if (yearInCycle > 0) {
        // Account for leap days in previous years of this cycle
        days -= (yearInCycle - 1) / 4;
    }
    year += yearInCycle;

    timeFields->Year = static_cast<uint16_t>(year);

    // Determine if current year is a leap year
    bool isLeapYear = ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);

    // Calculate month and day
    const int* monthDays = DaysInMonth[isLeapYear ? 1 : 0];
    uint32_t dayOfYear = static_cast<uint32_t>(days) + 1; // Convert to 1-based

    uint16_t month = 1;
    while (dayOfYear > static_cast<uint32_t>(monthDays[month - 1])) {
        dayOfYear -= monthDays[month - 1];
        month++;
    }

    timeFields->Month = month;
    timeFields->Day = static_cast<uint16_t>(dayOfYear);
}

void RtlFreeAnsiString()
{
    LOG_UTILITY("!!! STUB !!!");
}

void RtlUnicodeStringToAnsiString()
{
    LOG_UTILITY("!!! STUB !!!");
}

void RtlInitUnicodeString()
{
    LOG_UTILITY("!!! STUB !!!");
}

void ExTerminateThread(uint32_t exitCode)
{
    // Thread termination request. The calling thread should exit.
    // Since this is typically called for cleanup during VdInitializeEngines,
    // we can safely ignore it for now.
    (void)exitCode;
}

uint32_t ExCreateThread(be<uint32_t>* handle, uint32_t stackSize, be<uint32_t>* threadId, uint32_t xApiThreadStartup, uint32_t startAddress, uint32_t startContext, uint32_t creationFlags)
{

    uint32_t hostThreadId;

    uint32_t entry = startAddress;
    uint32_t r3 = startContext;
    uint32_t r4 = 0;

    // Many titles use an XAPI thread startup wrapper that takes (entry, arg) in (r3, r4)
    // and then performs an indirect call to entry.
    if (xApiThreadStartup != 0)
    {
        entry = xApiThreadStartup;
        r3 = startAddress;
        r4 = startContext;
    }

    // Debug: Dump memory at startContext to see what callback should be there
    if (startContext != 0 && startContext >= 0x82000000 && startContext < 0x84000000)
    {
        uint32_t* ctxMem = reinterpret_cast<uint32_t*>(g_memory.Translate(startContext));
        LOGF_IMPL(Utility, "ExCreateThread", "startContext@0x{:08X}: [0]=0x{:08X}, [1]=0x{:08X}, [2]=0x{:08X}, [3]=0x{:08X}",
            startContext, 
            ByteSwap(ctxMem[0]), ByteSwap(ctxMem[1]), ByteSwap(ctxMem[2]), ByteSwap(ctxMem[3]));
    }

    *handle = GetKernelHandle(GuestThread::Start({ entry, r3, r4, creationFlags }, &hostThreadId));
    LOGF_UTILITY("0x{:X}, 0x{:X}, 0x{:X}, 0x{:X}, 0x{:X}, 0x{:X}, 0x{:X} {:X}",
        (intptr_t)handle, stackSize, (intptr_t)threadId, xApiThreadStartup, startAddress, startContext, creationFlags, hostThreadId);
    if (threadId != nullptr)
        *threadId = hostThreadId;

    return 0;
}

void IoInvalidDeviceRequest()
{
    LOG_UTILITY("!!! STUB !!!");
}

void ObReferenceObject()
{
    LOG_UTILITY("!!! STUB !!!");
}

void IoCreateDevice()
{
    LOG_UTILITY("!!! STUB !!!");
}

void IoDeleteDevice()
{
    LOG_UTILITY("!!! STUB !!!");
}

void ExAllocatePoolTypeWithTag()
{
    LOG_UTILITY("!!! STUB !!!");
}

void RtlTimeFieldsToTime()
{
    LOG_UTILITY("!!! STUB !!!");
}

void IoCompleteRequest()
{
    LOG_UTILITY("!!! STUB !!!");
}

void RtlUpcaseUnicodeChar()
{
    LOG_UTILITY("!!! STUB !!!");
}

void ObIsTitleObject()
{
    LOG_UTILITY("!!! STUB !!!");
}

void IoCheckShareAccess()
{
    LOG_UTILITY("!!! STUB !!!");
}

void IoSetShareAccess()
{
    LOG_UTILITY("!!! STUB !!!");
}

void IoRemoveShareAccess()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_XNetStartup()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_XNetGetTitleXnAddr()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t KeWaitForMultipleObjects(uint32_t Count, xpointer<XDISPATCHER_HEADER>* Objects, uint32_t WaitType, uint32_t WaitReason, uint32_t WaitMode, uint32_t Alertable, be<int64_t>* Timeout)
{
    // FIXME: This function is only accounting for events.

    const uint64_t timeout = GuestTimeoutToMilliseconds(Timeout);
    assert(timeout == INFINITE);

    if (WaitType == 0) // Wait all
    {
        for (size_t i = 0; i < Count; i++)
            QueryKernelObject<Event>(*Objects[i])->Wait(timeout);
    }
    else
    {
        thread_local std::vector<Event*> s_events;
        s_events.resize(Count);

        for (size_t i = 0; i < Count; i++)
            s_events[i] = QueryKernelObject<Event>(*Objects[i]);

        while (true)
        {
            uint32_t generation = g_keSetEventGeneration.load();

            for (size_t i = 0; i < Count; i++)
            {
                if (s_events[i]->Wait(0) == STATUS_SUCCESS)
                {
                    return STATUS_WAIT_0 + i;
                }
            }

            g_keSetEventGeneration.wait(generation);
        }
    }

    return STATUS_SUCCESS;
}

uint32_t KeRaiseIrqlToDpcLevel()
{
    return 0;
}

void KfLowerIrql() { }

uint32_t KeReleaseSemaphore(XKSEMAPHORE* semaphore, uint32_t increment, uint32_t adjustment, uint32_t wait)
{
    auto* object = QueryKernelObject<Semaphore>(semaphore->Header);
    object->Release(adjustment, nullptr);
    return STATUS_SUCCESS;
}

void XAudioGetVoiceCategoryVolume()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t XAudioGetVoiceCategoryVolumeChangeMask(uint32_t Driver, be<uint32_t>* Mask)
{
    *Mask = 0;
    return 0;
}

uint32_t KeResumeThread(GuestThreadHandle* object)
{
    assert(object != GetKernelObject(CURRENT_THREAD_HANDLE));

    object->suspended = false;
    object->suspended.notify_all();
    return 0;
}

void KeInitializeSemaphore(XKSEMAPHORE* semaphore, uint32_t count, uint32_t limit)
{
    semaphore->Header.Type = 5;
    semaphore->Header.SignalState = count;
    semaphore->Limit = limit;

    auto* object = QueryKernelObject<Semaphore>(semaphore->Header);
}

void XMAReleaseContext()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XMACreateContext()
{
    LOG_UTILITY("!!! STUB !!!");
}

// uint32_t XAudioRegisterRenderDriverClient(be<uint32_t>* callback, be<uint32_t>* driver)
// {
//     //printf("XAudioRegisterRenderDriverClient(): %x %x\n");
// 
//     *driver = apu::RegisterClient(callback[0], callback[1]);
//     return 0;
// }

// void XAudioUnregisterRenderDriverClient()
// {
//     printf("!!! STUB !!! XAudioUnregisterRenderDriverClient\n");
// }

// uint32_t XAudioSubmitRenderDriverFrame(uint32_t driver, void* samples)
// {
//     // printf("!!! STUB !!! XAudioSubmitRenderDriverFrame\n");
//     apu::SubmitFrames(samples);
// 
//     return 0;
// }

void XapiInitProcess()
{
    printf("XapiInitProcess Invoked\n");

    int *XapiProcessHeap = (int *)g_memory.Translate(0x82D57540);

    *XapiProcessHeap = 1;
}

// GTA IV specific stubs
void XamTaskSchedule()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t XamTaskShouldExit(uint32_t taskHandle)
{
    // Return 1 to indicate task should exit (no async tasks supported yet)
    return 1;
}

uint32_t XamTaskCloseHandle(uint32_t taskHandle)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_SUCCESS;
}

uint32_t XamUserAreUsersFriends(uint32_t userIndex, uint64_t* xuids, uint32_t count, uint32_t* results, void* overlapped)
{
    // Return that no users are friends
    if (results)
        memset(results, 0, count * sizeof(uint32_t));
    return ERROR_SUCCESS;
}

uint32_t XamUserCheckPrivilege(uint32_t userIndex, uint32_t privilegeType, uint32_t* result)
{
    // Grant all privileges
    if (result)
        *result = 1;
    return ERROR_SUCCESS;
}

uint32_t XamUserCreateStatsEnumerator(uint32_t titleId, uint32_t userIndex, uint32_t xuidCount,
    void* pxuid, uint32_t dwNumStatSpecs, void* pStatSpecs, be<uint32_t>* pcbBuffer, be<uint32_t>* phEnum)
{
    // Return no stats available
    if (phEnum)
        *phEnum = 0;
    if (pcbBuffer)
        *pcbBuffer = 0;
    return ERROR_NO_MORE_FILES;
}

uint32_t XamUserGetName(uint32_t userIndex, char* buffer, uint32_t bufferSize)
{
    if (userIndex != 0)
        return ERROR_NO_SUCH_USER;
    if (buffer && bufferSize > 0)
    {
        const char* name = "Player";
        strncpy(buffer, name, bufferSize - 1);
        buffer[bufferSize - 1] = '\0';
    }
    return ERROR_SUCCESS;
}

uint32_t XamUserGetXUID(uint32_t userIndex, uint64_t* xuid)
{
    if (userIndex != 0)
        return ERROR_NO_SUCH_USER;
    if (xuid)
        *xuid = 0x0001000000000001ULL; // Fake XUID
    return ERROR_SUCCESS;
}

uint32_t XamUserWriteProfileSettings(uint32_t userIndex, uint32_t numSettings, void* settings, void* overlapped)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_SUCCESS;
}

uint32_t XamVoiceCreate(uint32_t userIndex, uint32_t flags, void** voice)
{
    LOG_UTILITY("!!! STUB !!!");
    if (voice)
        *voice = nullptr;
    return ERROR_DEVICE_NOT_CONNECTED;
}

void XamVoiceClose(void* voice)
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t XamVoiceHeadsetPresent(uint32_t userIndex)
{
    // No headset present
    return 0;
}

uint32_t XamVoiceSubmitPacket(void* voice, uint32_t size, void* data)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_SUCCESS;
}

uint32_t XeKeysConsolePrivateKeySign(void* signature, void* data, uint32_t dataSize)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_SUCCESS;
}

void IoDismountVolumeByFileHandle()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t NetDll_XNetGetEthernetLinkStatus()
{
    // Return link up
    return 1;
}

uint32_t KeTryToAcquireSpinLockAtRaisedIrql(void* spinlock)
{
    // Try to acquire, always succeed for now
    return 1;
}

uint32_t XamShowGamerCardUIForXUID(uint32_t userIndex, uint64_t xuid, void* overlapped)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_SUCCESS;
}

uint32_t XamShowPlayerReviewUI(uint32_t userIndex, uint64_t xuid, void* overlapped)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_SUCCESS;
}

// Additional GTA IV stubs
void* XamAlloc(uint32_t flags, uint32_t size, void* pBuffer)
{
    LOG_UTILITY("!!! STUB !!!");
    return nullptr;
}

void XamFree(void* buffer)
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t NtSetTimerEx(void* timerHandle, void* dueTime, void* apcRoutine, void* apcContext, uint32_t resume, uint32_t period, void* previousState)
{
    LOG_UTILITY("!!! STUB !!!");
    return 0;
}

uint32_t NtCancelTimer(void* timerHandle, void* currentState)
{
    LOG_UTILITY("!!! STUB !!!");
    return 0;
}

uint32_t NtCreateTimer(void* timerHandle, uint32_t desiredAccess, void* objectAttributes, uint32_t timerType)
{
    LOG_UTILITY("!!! STUB !!!");
    return 0;
}

uint32_t NtCreateMutant(void* mutantHandle, uint32_t desiredAccess, void* objectAttributes, uint32_t initialOwner)
{
    LOG_UTILITY("!!! STUB !!!");
    return 0;
}

uint32_t NtReleaseMutant(void* mutantHandle, void* previousCount)
{
    LOG_UTILITY("!!! STUB !!!");
    return 0;
}

void IoDismountVolume()
{
    LOG_UTILITY("!!! STUB !!!");
}

void XNotifyPositionUI(uint32_t position)
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_XNetCleanup()
{
    LOG_UTILITY("!!! STUB !!!");
}

int32_t NetDll_getsockname(uint32_t socket, void* name, void* namelen)
{
    LOG_UTILITY("!!! STUB !!!");
    return -1;
}

int32_t NetDll_ioctlsocket(uint32_t socket, uint32_t cmd, void* argp)
{
    LOG_UTILITY("!!! STUB !!!");
    return 0;
}

int32_t NetDll_sendto(uint32_t socket, void* buf, uint32_t len, uint32_t flags, void* to, uint32_t tolen)
{
    LOG_UTILITY("!!! STUB !!!");
    return -1;
}

int32_t NetDll_recvfrom(uint32_t socket, void* buf, uint32_t len, uint32_t flags, void* from, void* fromlen)
{
    LOG_UTILITY("!!! STUB !!!");
    return -1;
}

int32_t NetDll_shutdown(uint32_t socket, uint32_t how)
{
    LOG_UTILITY("!!! STUB !!!");
    return 0;
}

void XMsgCancelIORequest()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t XAudioGetSpeakerConfig(void* config)
{
    LOG_UTILITY("!!! STUB !!!");
    return 0;
}

uint32_t XamContentSetThumbnail(const char* rootName, void* imageData, uint32_t imageSize, void* overlapped)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_SUCCESS;
}

uint32_t XamInputGetKeystrokeEx(uint32_t userIndex, uint32_t flags, void* keystroke)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_DEVICE_NOT_CONNECTED;
}

uint32_t XamSessionCreateHandle(void* sessionInfo, void* handle)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_SUCCESS;
}

uint32_t XamSessionRefObjByHandle(uint32_t handle, void* obj)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_INVALID_HANDLE;
}

void KeSetDisableBoostThread(void* thread, uint32_t disable)
{
    // Thread priority boost control is not implemented.
    // This is safe to ignore as it only affects scheduling hints.
    (void)thread;
    (void)disable;
}

uint32_t XamCreateEnumeratorHandle(uint32_t type, uint32_t size, void* data, void* handle)
{
    LOG_UTILITY("!!! STUB !!!");
    return ERROR_SUCCESS;
}

uint32_t NtDeviceIoControlFile(void* fileHandle, void* event, void* apcRoutine, void* apcContext, void* ioStatusBlock, uint32_t ioControlCode, void* inputBuffer, uint32_t inputBufferLength, void* outputBuffer, uint32_t outputBufferLength)
{
    LOG_UTILITY("!!! STUB !!!");
    return 0;
}

int32_t NetDll_WSAGetLastError()
{
    LOG_UTILITY("!!! STUB !!!");
    return 0;
}

void NetDll_XNetQosListen()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_XNetQosLookup()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_XNetQosRelease()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_XNetServerToInAddr()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_XNetXnAddrToInAddr()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_XNetGetConnectStatus()
{
    LOG_UTILITY("!!! STUB !!!");
}

void NetDll_XNetUnregisterInAddr()
{
    LOG_UTILITY("!!! STUB !!!");
}

// =============================================================================
// GTA IV: Bypass dirty disc error display only
// sub_82192100 - dirty disc error display UI - HOOK to skip UI
// sub_829A1290 - another dirty disc caller
// sub_827F0B20 - retry loop - let it run but limit retries
// sub_829A1F00 - the actual file loading function
// =============================================================================

// Hook sub_82192100 - skip dirty disc error UI and limit retries
PPC_FUNC(sub_82192100)
{
    static int count = 0;
    count++;
    
    if (count <= 5)
    {
        LOGF_IMPL(Utility, "GTA4_DirtyDisc", "sub_82192100 (dirty disc UI) called #{} - skipping", count);
    }
    
    // After too many calls, log and let game continue without crashing
    if (count >= 50)
    {
        LOG_UTILITY("[GTA4_DirtyDisc] Too many dirty disc errors - game may have issues loading files");
    }
    
    // Return without showing UI - just skip
}

// Hook sub_829A1290 - skip dirty disc caller
PPC_FUNC(sub_829A1290)
{
    static int count = 0;
    if (++count <= 3)
    {
        LOG_UTILITY("[GTA4] sub_829A1290 (dirty disc caller) - skipping");
    }
}

// Forward declare the original implementation from recompiled code
extern "C" void __imp__sub_829A1F00(PPCContext& ctx, uint8_t* base);

// Hook sub_829A1F00 - the actual file loading function
// This function is called by the game's internal packfile system.
// We serve data from RPF files with decrypted TOC, or fall back to original implementation.
// Parameters: r3=handle, r4=buffer(guest), r5=size, r6=offset, r7=asyncInfo
PPC_FUNC(sub_829A1F00)
{
    static int count = 0;
    count++;
    
    const uint32_t handle = ctx.r3.u32;
    const uint32_t guestBuffer = ctx.r4.u32;
    const uint32_t size = ctx.r5.u32;
    const uint32_t offset = ctx.r6.u32;
    const uint32_t asyncInfo = ctx.r7.u32;
    
    uint8_t* hostBuffer = reinterpret_cast<uint8_t*>(base + guestBuffer);
    
    // Log first 50 calls and then every 100th
    if (count <= 50 || count % 100 == 0)
    {
        LOGF_IMPL(Utility, "GTA4_FileLoad", "sub_829A1F00 #{} handle=0x{:08X} size=0x{:X} offset=0x{:X} async=0x{:08X}",
                  count, handle, size, offset, asyncInfo);
    }
    
    // For ASYNC reads: serve data ourselves, but also update the REQUEST OBJECT
    // The async struct at asyncInfo has [5] pointing to a request object (0x000A00F0)
    // That request object likely has a state field we need to set to COMPLETED
    uint32_t* asyncPtr = nullptr;
    uint32_t requestObjAddr = 0;
    if (asyncInfo != 0 && asyncInfo < 0x20000000)
    {
        asyncPtr = reinterpret_cast<uint32_t*>(base + asyncInfo);
        requestObjAddr = ByteSwap(asyncPtr[5]);  // Request object address
        
        if (count <= 20)
        {
            // Dump the REQUEST OBJECT to find the state field
            if (requestObjAddr != 0 && requestObjAddr < 0x20000000)
            {
                uint32_t* reqObj = reinterpret_cast<uint32_t*>(base + requestObjAddr);
                LOGF_IMPL(Utility, "GTA4_FileLoad", "REQUEST_OBJ at 0x{:08X}: [0-3]=0x{:08X} 0x{:08X} 0x{:08X} 0x{:08X}",
                    requestObjAddr, ByteSwap(reqObj[0]), ByteSwap(reqObj[1]), ByteSwap(reqObj[2]), ByteSwap(reqObj[3]));
                LOGF_IMPL(Utility, "GTA4_FileLoad", "REQUEST_OBJ at 0x{:08X}: [4-7]=0x{:08X} 0x{:08X} 0x{:08X} 0x{:08X}",
                    requestObjAddr, ByteSwap(reqObj[4]), ByteSwap(reqObj[5]), ByteSwap(reqObj[6]), ByteSwap(reqObj[7]));
            }
        }
    }
    
    // Translation layer: serve from RPF streams with decrypted TOC
    {
        std::string chosen;
        const uint32_t bytesRead = ReadFromBestRpf(handle, hostBuffer, size, offset, count, chosen);
        if (bytesRead > 0)
        {
            g_handleToRpf[handle] = chosen;
            if (count <= 50 || count % 100 == 0)
            {
                LOGF_IMPL(Utility, "GTA4_FileLoad", "sub_829A1F00 #{} read {} bytes from '{}' at offset 0x{:X}",
                          count, bytesRead, chosen, offset);
            }
            
            // For async reads, try returning bytesRead directly
            // The game might check r3 for the actual byte count to determine completion
            if (asyncInfo != 0)
            {
                ctx.r3.u32 = bytesRead;  // Return bytes read for async
                if (count <= 20)
                {
                    LOGF_IMPL(Utility, "GTA4_FileLoad", "ASYNC: returning bytesRead={} in r3", bytesRead);
                }
            }
            else
            {
                ctx.r3.u32 = 1;  // Return 1 for sync reads
            }
            return;
        }
    }
    
    // Check if this is one of our file handles
    auto it = g_ntFileHandles.find(handle);
    if (it != g_ntFileHandles.end() && it->second && it->second->magic == kNtFileHandleMagic)
    {
        NtFileHandle* hFile = it->second;
        
        // Parse RPF header if not done yet
        if (!hFile->rpfHeaderParsed)
            ParseRpfHeader(hFile);
        
        // Seek to offset and read
        hFile->stream.clear();
        hFile->stream.seekg(offset, std::ios::beg);
        
        if (!hFile->stream.bad())
        {
            hFile->stream.read(reinterpret_cast<char*>(hostBuffer), size);
            const uint32_t bytesRead = static_cast<uint32_t>(hFile->stream.gcount());
            
            // Decrypt TOC region if this is an encrypted RPF
            if (hFile->isRpf && hFile->tocEncrypted && bytesRead > 0)
            {
                const uint64_t tocStart = hFile->tocOffset;
                const uint64_t tocEnd = tocStart + hFile->tocSize;
                const uint64_t readStart = offset;
                const uint64_t readEnd = offset + bytesRead;
                const uint64_t overlapStart = std::max(readStart, tocStart);
                const uint64_t overlapEnd = std::min(readEnd, tocEnd);
                
                if (overlapEnd > overlapStart)
                {
                    const uint32_t startInBuf = static_cast<uint32_t>(overlapStart - readStart);
                    const uint32_t len = static_cast<uint32_t>(overlapEnd - overlapStart);
                    const uint64_t tocRelativeOffset = overlapStart - tocStart;
                    
                    if (count <= 20 || count % 100 == 0)
                    {
                        LOGF_IMPL(Utility, "GTA4_FileLoad", "Decrypting TOC region: bufOffset={} len={} tocRelOffset={}",
                                  startInBuf, len, tocRelativeOffset);
                    }
                    
                    DecryptRpfBufferInPlace(hostBuffer + startInBuf, len, tocRelativeOffset);
                }
            }
            
            if (count <= 20 || count % 100 == 0)
            {
                LOGF_IMPL(Utility, "GTA4_FileLoad", "sub_829A1F00 #{} read {} bytes from '{}' at offset 0x{:X}",
                          count, bytesRead, hFile->path.filename().string(), offset);
            }
            
            if (bytesRead > 0)
            {
                ctx.r3.u32 = 1;
                return;
            }
        }
    }

    // Check if this is a virtual file handle
    auto vit = g_ntVirtFileHandles.find(handle);
    if (vit != g_ntVirtFileHandles.end() && vit->second && vit->second->magic == kNtVirtFileHandleMagic)
    {
        NtVirtFileHandle* hVirt = vit->second;
        
        // Fallback to virtual data if RPF read failed
        if (offset < hVirt->data.size())
        {
            const uint32_t available = static_cast<uint32_t>(hVirt->data.size() - offset);
            const uint32_t toCopy = std::min(size, available);
            memcpy(hostBuffer, hVirt->data.data() + offset, toCopy);
            
            if (count <= 20 || count % 100 == 0)
            {
                LOGF_IMPL(Utility, "GTA4_FileLoad", "sub_829A1F00 #{} read {} bytes from virtual data at offset 0x{:X}",
                          count, toCopy, offset);
            }
            
            ctx.r3.u32 = 1;
            return;
        }
    }

    // All else failed
    if (count <= 20 || count % 100 == 0)
    {
        LOGF_IMPL(Utility, "GTA4_FileLoad", "sub_829A1F00 #{} FAILED for handle 0x{:08X}", count, handle);
    }
    ctx.r3.u32 = 0;
}

// DON'T hook sub_827F0B20 - let the retry loop run, our file system should handle it
GUEST_FUNCTION_HOOK(sub_825383D8, XapiInitProcess)
GUEST_FUNCTION_HOOK(__imp__XGetVideoMode, VdQueryVideoMode); // XGetVideoMode
GUEST_FUNCTION_HOOK(__imp__XNotifyGetNext, XNotifyGetNext);
GUEST_FUNCTION_HOOK(__imp__XGetGameRegion, XGetGameRegion);
GUEST_FUNCTION_HOOK(__imp__XMsgStartIORequest, XMsgStartIORequest);
GUEST_FUNCTION_HOOK(__imp__XamUserGetSigninState, XamUserGetSigninState);
GUEST_FUNCTION_HOOK(__imp__XamGetSystemVersion, XamGetSystemVersion);
GUEST_FUNCTION_HOOK(__imp__XamContentCreateEx, XamContentCreateEx);
GUEST_FUNCTION_HOOK(__imp__XamContentDelete, XamContentDelete);
GUEST_FUNCTION_HOOK(__imp__XamContentClose, XamContentClose);
GUEST_FUNCTION_HOOK(__imp__XamContentGetCreator, XamContentGetCreator);
GUEST_FUNCTION_HOOK(__imp__XamContentCreateEnumerator, XamContentCreateEnumerator);
GUEST_FUNCTION_HOOK(__imp__XamContentGetDeviceState, XamContentGetDeviceState);
GUEST_FUNCTION_HOOK(__imp__XamContentGetDeviceData, XamContentGetDeviceData);
GUEST_FUNCTION_HOOK(__imp__XamEnumerate, XamEnumerate);
GUEST_FUNCTION_HOOK(__imp__XamNotifyCreateListener, XamNotifyCreateListener);
GUEST_FUNCTION_HOOK(__imp__XamUserGetSigninInfo, XamUserGetSigninInfo);
GUEST_FUNCTION_HOOK(__imp__XamShowSigninUI, XamShowSigninUI);
GUEST_FUNCTION_HOOK(__imp__XamShowDeviceSelectorUI, XamShowDeviceSelectorUI);
GUEST_FUNCTION_HOOK(__imp__XamShowMessageBoxUI, XamShowMessageBoxUI);
GUEST_FUNCTION_HOOK(__imp__XamUserCreateAchievementEnumerator, XamUserCreateAchievementEnumerator);
GUEST_FUNCTION_HOOK(__imp__XeKeysConsoleSignatureVerification, XeKeysConsoleSignatureVerification);
GUEST_FUNCTION_HOOK(__imp__XamGetPrivateEnumStructureFromHandle, XamGetPrivateEnumStructureFromHandle);
GUEST_FUNCTION_HOOK(__imp__XamTaskSchedule, XamTaskSchedule);
GUEST_FUNCTION_HOOK(__imp__XamTaskShouldExit, XamTaskShouldExit);
GUEST_FUNCTION_HOOK(__imp__XamTaskCloseHandle, XamTaskCloseHandle);
GUEST_FUNCTION_HOOK(__imp__XamUserAreUsersFriends, XamUserAreUsersFriends);
GUEST_FUNCTION_HOOK(__imp__XamUserCheckPrivilege, XamUserCheckPrivilege);
GUEST_FUNCTION_HOOK(__imp__XamUserCreateStatsEnumerator, XamUserCreateStatsEnumerator);
GUEST_FUNCTION_HOOK(__imp__XamUserGetName, XamUserGetName);
GUEST_FUNCTION_HOOK(__imp__XamUserGetXUID, XamUserGetXUID);
GUEST_FUNCTION_HOOK(__imp__XamUserWriteProfileSettings, XamUserWriteProfileSettings);
GUEST_FUNCTION_HOOK(__imp__XamVoiceCreate, XamVoiceCreate);
GUEST_FUNCTION_HOOK(__imp__XamVoiceClose, XamVoiceClose);
GUEST_FUNCTION_HOOK(__imp__XamVoiceHeadsetPresent, XamVoiceHeadsetPresent);
GUEST_FUNCTION_HOOK(__imp__XamVoiceSubmitPacket, XamVoiceSubmitPacket);
GUEST_FUNCTION_HOOK(__imp__XeKeysConsolePrivateKeySign, XeKeysConsolePrivateKeySign);
GUEST_FUNCTION_HOOK(__imp__IoDismountVolumeByFileHandle, IoDismountVolumeByFileHandle);
GUEST_FUNCTION_HOOK(__imp__NetDll_XNetGetEthernetLinkStatus, NetDll_XNetGetEthernetLinkStatus);
GUEST_FUNCTION_HOOK(__imp__KeTryToAcquireSpinLockAtRaisedIrql, KeTryToAcquireSpinLockAtRaisedIrql);
GUEST_FUNCTION_HOOK(__imp__XamShowGamerCardUIForXUID, XamShowGamerCardUIForXUID);
GUEST_FUNCTION_HOOK(__imp__XamShowPlayerReviewUI, XamShowPlayerReviewUI);
GUEST_FUNCTION_HOOK(__imp__XamShowDirtyDiscErrorUI, XamShowDirtyDiscErrorUI);
GUEST_FUNCTION_HOOK(__imp__XamEnableInactivityProcessing, XamEnableInactivityProcessing);
GUEST_FUNCTION_HOOK(__imp__XamResetInactivity, XamResetInactivity);
GUEST_FUNCTION_HOOK(__imp__XamShowMessageBoxUIEx, XamShowMessageBoxUIEx);
GUEST_FUNCTION_HOOK(__imp__XGetLanguage, XGetLanguage);
GUEST_FUNCTION_HOOK(__imp__XGetAVPack, XGetAVPack);
GUEST_FUNCTION_HOOK(__imp__XamLoaderTerminateTitle, XamLoaderTerminateTitle);
GUEST_FUNCTION_HOOK(__imp__XamGetExecutionId, XamGetExecutionId);
GUEST_FUNCTION_HOOK(__imp__XamLoaderLaunchTitle, XamLoaderLaunchTitle);
GUEST_FUNCTION_HOOK(__imp__NtOpenFile, NtOpenFile);
GUEST_FUNCTION_HOOK(__imp__RtlInitAnsiString, RtlInitAnsiString);
GUEST_FUNCTION_HOOK(__imp__NtCreateFile, NtCreateFile);
GUEST_FUNCTION_HOOK(__imp__NtClose, NtClose);
GUEST_FUNCTION_HOOK(__imp__NtSetInformationFile, NtSetInformationFile);
GUEST_FUNCTION_HOOK(__imp__FscGetCacheElementCount, FscGetCacheElementCount);
GUEST_FUNCTION_HOOK(__imp__FscSetCacheElementCount, FscSetCacheElementCount);
GUEST_FUNCTION_HOOK(__imp__XamLoaderGetLaunchDataSize, XamLoaderGetLaunchDataSize);
GUEST_FUNCTION_HOOK(__imp__XamLoaderGetLaunchData, XamLoaderGetLaunchData);
GUEST_FUNCTION_HOOK(__imp__XamLoaderSetLaunchData, XamLoaderSetLaunchData);
GUEST_FUNCTION_HOOK(__imp__NtWaitForSingleObjectEx, NtWaitForSingleObjectEx);
GUEST_FUNCTION_HOOK(__imp__NtWriteFile, NtWriteFile);
GUEST_FUNCTION_HOOK(__imp__ExGetXConfigSetting, ExGetXConfigSetting);
GUEST_FUNCTION_HOOK(__imp__NtQueryVirtualMemory, NtQueryVirtualMemory);
GUEST_FUNCTION_HOOK(__imp__MmQueryStatistics, MmQueryStatistics);
GUEST_FUNCTION_HOOK(__imp__NtCreateEvent, NtCreateEvent);
GUEST_FUNCTION_HOOK(__imp__XexCheckExecutablePrivilege, XexCheckExecutablePrivilege);
GUEST_FUNCTION_HOOK(__imp__DbgPrint, DbgPrint);
GUEST_FUNCTION_HOOK(__imp____C_specific_handler, __C_specific_handler_x);
GUEST_FUNCTION_HOOK(__imp__RtlNtStatusToDosError, RtlNtStatusToDosError);
GUEST_FUNCTION_HOOK(__imp__XexGetProcedureAddress, XexGetProcedureAddress);
GUEST_FUNCTION_HOOK(__imp__XexGetModuleSection, XexGetModuleSection);
GUEST_FUNCTION_HOOK(__imp__RtlUnicodeToMultiByteN, RtlUnicodeToMultiByteN);
GUEST_FUNCTION_HOOK(__imp__KeDelayExecutionThread, KeDelayExecutionThread);
GUEST_FUNCTION_HOOK(__imp__ExFreePool, ExFreePool);
GUEST_FUNCTION_HOOK(__imp__NtQueryInformationFile, NtQueryInformationFile);
GUEST_FUNCTION_HOOK(__imp__NtQueryVolumeInformationFile, NtQueryVolumeInformationFile);
GUEST_FUNCTION_HOOK(__imp__NtQueryDirectoryFile, NtQueryDirectoryFile);
GUEST_FUNCTION_HOOK(__imp__NtReadFileScatter, NtReadFileScatter);
GUEST_FUNCTION_HOOK(__imp__NtReadFile, NtReadFile);
GUEST_FUNCTION_HOOK(__imp__NtDuplicateObject, NtDuplicateObject);
GUEST_FUNCTION_HOOK(__imp__NtAllocateVirtualMemory, NtAllocateVirtualMemory);
GUEST_FUNCTION_HOOK(__imp__NtFreeVirtualMemory, NtFreeVirtualMemory);
GUEST_FUNCTION_HOOK(__imp__ObDereferenceObject, ObDereferenceObject);
GUEST_FUNCTION_HOOK(__imp__KeSetBasePriorityThread, KeSetBasePriorityThread);
GUEST_FUNCTION_HOOK(__imp__ObReferenceObjectByHandle, ObReferenceObjectByHandle);
GUEST_FUNCTION_HOOK(__imp__KeQueryBasePriorityThread, KeQueryBasePriorityThread);
GUEST_FUNCTION_HOOK(__imp__NtSuspendThread, NtSuspendThread);
GUEST_FUNCTION_HOOK(__imp__KeSetAffinityThread, KeSetAffinityThread);
GUEST_FUNCTION_HOOK(__imp__RtlLeaveCriticalSection, RtlLeaveCriticalSection);
GUEST_FUNCTION_HOOK(__imp__RtlEnterCriticalSection, RtlEnterCriticalSection);
GUEST_FUNCTION_HOOK(__imp__RtlImageXexHeaderField, RtlImageXexHeaderField);
GUEST_FUNCTION_HOOK(__imp__HalReturnToFirmware, HalReturnToFirmware);
GUEST_FUNCTION_HOOK(__imp__RtlFillMemoryUlong, RtlFillMemoryUlong);
GUEST_FUNCTION_HOOK(__imp__KeBugCheckEx, KeBugCheckEx);
GUEST_FUNCTION_HOOK(__imp__KeGetCurrentProcessType, KeGetCurrentProcessType);
GUEST_FUNCTION_HOOK(__imp__RtlCompareMemoryUlong, RtlCompareMemoryUlong);
GUEST_FUNCTION_HOOK(__imp__RtlInitializeCriticalSection, RtlInitializeCriticalSection);
GUEST_FUNCTION_HOOK(__imp__RtlRaiseException, RtlRaiseException_x);
GUEST_FUNCTION_HOOK(__imp__KfReleaseSpinLock, KfReleaseSpinLock);
GUEST_FUNCTION_HOOK(__imp__KfAcquireSpinLock, KfAcquireSpinLock);
GUEST_FUNCTION_HOOK(__imp__KeQueryPerformanceFrequency, KeQueryPerformanceFrequency);
GUEST_FUNCTION_HOOK(__imp__MmFreePhysicalMemory, MmFreePhysicalMemory);
GUEST_FUNCTION_HOOK(__imp__VdPersistDisplay, VdPersistDisplay);
GUEST_FUNCTION_HOOK(__imp__VdSwap, VdSwap);
GUEST_FUNCTION_HOOK(__imp__VdGetSystemCommandBuffer, VdGetSystemCommandBuffer);
GUEST_FUNCTION_HOOK(__imp__KeReleaseSpinLockFromRaisedIrql, KeReleaseSpinLockFromRaisedIrql);
GUEST_FUNCTION_HOOK(__imp__KeAcquireSpinLockAtRaisedIrql, KeAcquireSpinLockAtRaisedIrql);
GUEST_FUNCTION_HOOK(__imp__KiApcNormalRoutineNop, KiApcNormalRoutineNop);
GUEST_FUNCTION_HOOK(__imp__VdEnableRingBufferRPtrWriteBack, VdEnableRingBufferRPtrWriteBack);
GUEST_FUNCTION_HOOK(__imp__VdInitializeRingBuffer, VdInitializeRingBuffer);
GUEST_FUNCTION_HOOK(__imp__MmGetPhysicalAddress, MmGetPhysicalAddress);
GUEST_FUNCTION_HOOK(__imp__VdSetSystemCommandBufferGpuIdentifierAddress, VdSetSystemCommandBufferGpuIdentifierAddress);
GUEST_FUNCTION_HOOK(__imp__ExRegisterTitleTerminateNotification, ExRegisterTitleTerminateNotification);
GUEST_FUNCTION_HOOK(__imp__VdShutdownEngines, VdShutdownEngines);
GUEST_FUNCTION_HOOK(__imp__VdQueryVideoMode, VdQueryVideoMode);
GUEST_FUNCTION_HOOK(__imp__VdGetCurrentDisplayInformation, VdGetCurrentDisplayInformation);
GUEST_FUNCTION_HOOK(__imp__VdSetDisplayMode, VdSetDisplayMode);
GUEST_FUNCTION_HOOK(__imp__VdSetGraphicsInterruptCallback, VdSetGraphicsInterruptCallback);
GUEST_FUNCTION_HOOK(__imp__VdInitializeEngines, VdInitializeEngines);
GUEST_FUNCTION_HOOK(__imp__VdIsHSIOTrainingSucceeded, VdIsHSIOTrainingSucceeded);
GUEST_FUNCTION_HOOK(__imp__VdGetCurrentDisplayGamma, VdGetCurrentDisplayGamma);
GUEST_FUNCTION_HOOK(__imp__VdQueryVideoFlags, VdQueryVideoFlags);
GUEST_FUNCTION_HOOK(__imp__VdCallGraphicsNotificationRoutines, VdCallGraphicsNotificationRoutines);
GUEST_FUNCTION_HOOK(__imp__VdInitializeScalerCommandBuffer, VdInitializeScalerCommandBuffer);
GUEST_FUNCTION_HOOK(__imp__KeLeaveCriticalRegion, KeLeaveCriticalRegion);
GUEST_FUNCTION_HOOK(__imp__VdRetrainEDRAM, VdRetrainEDRAM);
GUEST_FUNCTION_HOOK(__imp__VdRetrainEDRAMWorker, VdRetrainEDRAMWorker);
GUEST_FUNCTION_HOOK(__imp__KeEnterCriticalRegion, KeEnterCriticalRegion);
GUEST_FUNCTION_HOOK(__imp__MmAllocatePhysicalMemoryEx, MmAllocatePhysicalMemoryEx);
GUEST_FUNCTION_HOOK(__imp__ObDeleteSymbolicLink, ObDeleteSymbolicLink);
GUEST_FUNCTION_HOOK(__imp__ObCreateSymbolicLink, ObCreateSymbolicLink);
GUEST_FUNCTION_HOOK(__imp__MmQueryAddressProtect, MmQueryAddressProtect);
GUEST_FUNCTION_HOOK(__imp__VdEnableDisableClockGating, VdEnableDisableClockGating);
GUEST_FUNCTION_HOOK(__imp__KeBugCheck, KeBugCheck);
GUEST_FUNCTION_HOOK(__imp__KeLockL2, KeLockL2);
GUEST_FUNCTION_HOOK(__imp__KeUnlockL2, KeUnlockL2);
GUEST_FUNCTION_HOOK(__imp__KeSetEvent, KeSetEvent);
GUEST_FUNCTION_HOOK(__imp__KeResetEvent, KeResetEvent);
GUEST_FUNCTION_HOOK(__imp__KeWaitForSingleObject, KeWaitForSingleObject);
GUEST_FUNCTION_HOOK(__imp__KeTlsGetValue, KeTlsGetValue);
GUEST_FUNCTION_HOOK(__imp__KeTlsSetValue, KeTlsSetValue);
GUEST_FUNCTION_HOOK(__imp__KeTlsAlloc, KeTlsAlloc);
GUEST_FUNCTION_HOOK(__imp__KeTlsFree, KeTlsFree);
GUEST_FUNCTION_HOOK(__imp__XMsgInProcessCall, XMsgInProcessCall);
GUEST_FUNCTION_HOOK(__imp__XamUserReadProfileSettings, XamUserReadProfileSettings);
GUEST_FUNCTION_HOOK(__imp__NetDll_WSAStartup, NetDll_WSAStartup);
GUEST_FUNCTION_HOOK(__imp__NetDll_WSACleanup, NetDll_WSACleanup);
GUEST_FUNCTION_HOOK(__imp__NetDll_socket, NetDll_socket);
GUEST_FUNCTION_HOOK(__imp__NetDll_closesocket, NetDll_closesocket);
GUEST_FUNCTION_HOOK(__imp__NetDll_setsockopt, NetDll_setsockopt);
GUEST_FUNCTION_HOOK(__imp__NetDll_bind, NetDll_bind);
GUEST_FUNCTION_HOOK(__imp__NetDll_connect, NetDll_connect);
GUEST_FUNCTION_HOOK(__imp__NetDll_listen, NetDll_listen);
GUEST_FUNCTION_HOOK(__imp__NetDll_accept, NetDll_accept);
GUEST_FUNCTION_HOOK(__imp__NetDll_select, NetDll_select);
GUEST_FUNCTION_HOOK(__imp__NetDll_recv, NetDll_recv);
GUEST_FUNCTION_HOOK(__imp__NetDll_send, NetDll_send);
GUEST_FUNCTION_HOOK(__imp__NetDll_inet_addr, NetDll_inet_addr);
GUEST_FUNCTION_HOOK(__imp__NetDll___WSAFDIsSet, NetDll___WSAFDIsSet);
GUEST_FUNCTION_HOOK(__imp__XMsgStartIORequestEx, XMsgStartIORequestEx);
GUEST_FUNCTION_HOOK(__imp__XamInputGetCapabilities, XamInputGetCapabilities);
GUEST_FUNCTION_HOOK(__imp__XamInputGetState, XamInputGetState);
GUEST_FUNCTION_HOOK(__imp__XamInputSetState, XamInputSetState);
GUEST_FUNCTION_HOOK(__imp__XexGetModuleHandle, XexGetModuleHandle);
GUEST_FUNCTION_HOOK(__imp__RtlTryEnterCriticalSection, RtlTryEnterCriticalSection);
GUEST_FUNCTION_HOOK(__imp__RtlInitializeCriticalSectionAndSpinCount, RtlInitializeCriticalSectionAndSpinCount);
GUEST_FUNCTION_HOOK(__imp__XeCryptBnQwBeSigVerify, XeCryptBnQwBeSigVerify);
GUEST_FUNCTION_HOOK(__imp__XeKeysGetKey, XeKeysGetKey);
GUEST_FUNCTION_HOOK(__imp__XeCryptRotSumSha, XeCryptRotSumSha);
GUEST_FUNCTION_HOOK(__imp__XeCryptSha, XeCryptSha);
GUEST_FUNCTION_HOOK(__imp__KeEnableFpuExceptions, KeEnableFpuExceptions);
GUEST_FUNCTION_HOOK(__imp__RtlUnwind, RtlUnwind_x);
GUEST_FUNCTION_HOOK(__imp__RtlCaptureContext, RtlCaptureContext_x);
GUEST_FUNCTION_HOOK(__imp__NtQueryFullAttributesFile, NtQueryFullAttributesFile);
GUEST_FUNCTION_HOOK(__imp__RtlMultiByteToUnicodeN, RtlMultiByteToUnicodeN);
GUEST_FUNCTION_HOOK(__imp__DbgBreakPoint, DbgBreakPoint);
GUEST_FUNCTION_HOOK(__imp__MmQueryAllocationSize, MmQueryAllocationSize);
GUEST_FUNCTION_HOOK(__imp__NtClearEvent, NtClearEvent);
GUEST_FUNCTION_HOOK(__imp__NtResumeThread, NtResumeThread);
GUEST_FUNCTION_HOOK(__imp__NtSetEvent, NtSetEvent);
GUEST_FUNCTION_HOOK(__imp__NtCreateSemaphore, NtCreateSemaphore);
GUEST_FUNCTION_HOOK(__imp__NtReleaseSemaphore, NtReleaseSemaphore);
GUEST_FUNCTION_HOOK(__imp__NtWaitForMultipleObjectsEx, NtWaitForMultipleObjectsEx);
GUEST_FUNCTION_HOOK(__imp__RtlCompareStringN, RtlCompareStringN);
GUEST_FUNCTION_HOOK(__imp__StfsControlDevice, StfsControlDevice);
GUEST_FUNCTION_HOOK(__imp__StfsCreateDevice, StfsCreateDevice);
GUEST_FUNCTION_HOOK(__imp__NtFlushBuffersFile, NtFlushBuffersFile);
GUEST_FUNCTION_HOOK(__imp__KeQuerySystemTime, KeQuerySystemTime);
GUEST_FUNCTION_HOOK(__imp__RtlTimeToTimeFields, RtlTimeToTimeFields);
GUEST_FUNCTION_HOOK(__imp__RtlFreeAnsiString, RtlFreeAnsiString);
GUEST_FUNCTION_HOOK(__imp__RtlUnicodeStringToAnsiString, RtlUnicodeStringToAnsiString);
GUEST_FUNCTION_HOOK(__imp__RtlInitUnicodeString, RtlInitUnicodeString);
GUEST_FUNCTION_HOOK(__imp__ExTerminateThread, ExTerminateThread);
GUEST_FUNCTION_HOOK(__imp__ExCreateThread, ExCreateThread);
GUEST_FUNCTION_HOOK(__imp__IoInvalidDeviceRequest, IoInvalidDeviceRequest);
GUEST_FUNCTION_HOOK(__imp__ObReferenceObject, ObReferenceObject);
GUEST_FUNCTION_HOOK(__imp__IoCreateDevice, IoCreateDevice);
GUEST_FUNCTION_HOOK(__imp__IoDeleteDevice, IoDeleteDevice);
GUEST_FUNCTION_HOOK(__imp__ExAllocatePoolTypeWithTag, ExAllocatePoolTypeWithTag);
GUEST_FUNCTION_HOOK(__imp__RtlTimeFieldsToTime, RtlTimeFieldsToTime);
GUEST_FUNCTION_HOOK(__imp__IoCompleteRequest, IoCompleteRequest);
GUEST_FUNCTION_HOOK(__imp__RtlUpcaseUnicodeChar, RtlUpcaseUnicodeChar);
GUEST_FUNCTION_HOOK(__imp__ObIsTitleObject, ObIsTitleObject);
GUEST_FUNCTION_HOOK(__imp__IoCheckShareAccess, IoCheckShareAccess);
GUEST_FUNCTION_HOOK(__imp__IoSetShareAccess, IoSetShareAccess);
GUEST_FUNCTION_HOOK(__imp__IoRemoveShareAccess, IoRemoveShareAccess);
GUEST_FUNCTION_HOOK(__imp__NetDll_XNetStartup, NetDll_XNetStartup);
GUEST_FUNCTION_HOOK(__imp__NetDll_XNetGetTitleXnAddr, NetDll_XNetGetTitleXnAddr);
GUEST_FUNCTION_HOOK(__imp__KeWaitForMultipleObjects, KeWaitForMultipleObjects);
GUEST_FUNCTION_HOOK(__imp__KeRaiseIrqlToDpcLevel, KeRaiseIrqlToDpcLevel);
GUEST_FUNCTION_HOOK(__imp__KfLowerIrql, KfLowerIrql);
GUEST_FUNCTION_HOOK(__imp__KeReleaseSemaphore, KeReleaseSemaphore);
GUEST_FUNCTION_HOOK(__imp__XAudioGetVoiceCategoryVolume, XAudioGetVoiceCategoryVolume);
GUEST_FUNCTION_HOOK(__imp__XAudioGetVoiceCategoryVolumeChangeMask, XAudioGetVoiceCategoryVolumeChangeMask);
GUEST_FUNCTION_HOOK(__imp__KeResumeThread, KeResumeThread);
GUEST_FUNCTION_HOOK(__imp__KeInitializeSemaphore, KeInitializeSemaphore);
GUEST_FUNCTION_HOOK(__imp__XMAReleaseContext, XMAReleaseContext);
GUEST_FUNCTION_HOOK(__imp__XMACreateContext, XMACreateContext);
GUEST_FUNCTION_HOOK(__imp__XAudioRegisterRenderDriverClient, XAudioRegisterRenderDriverClient);
GUEST_FUNCTION_HOOK(__imp__XAudioUnregisterRenderDriverClient, XAudioUnregisterRenderDriverClient);
GUEST_FUNCTION_HOOK(__imp__XAudioSubmitRenderDriverFrame, XAudioSubmitRenderDriverFrame);
// Additional GTA IV stubs
GUEST_FUNCTION_HOOK(__imp__XamAlloc, XamAlloc);
GUEST_FUNCTION_HOOK(__imp__XamFree, XamFree);
GUEST_FUNCTION_HOOK(__imp__NtSetTimerEx, NtSetTimerEx);
GUEST_FUNCTION_HOOK(__imp__NtCancelTimer, NtCancelTimer);
GUEST_FUNCTION_HOOK(__imp__NtCreateTimer, NtCreateTimer);
GUEST_FUNCTION_HOOK(__imp__NtCreateMutant, NtCreateMutant);
GUEST_FUNCTION_HOOK(__imp__NtReleaseMutant, NtReleaseMutant);
GUEST_FUNCTION_HOOK(__imp__IoDismountVolume, IoDismountVolume);
GUEST_FUNCTION_HOOK(__imp__XNotifyPositionUI, XNotifyPositionUI);
GUEST_FUNCTION_HOOK(__imp__NetDll_XNetCleanup, NetDll_XNetCleanup);
GUEST_FUNCTION_HOOK(__imp__NetDll_getsockname, NetDll_getsockname);
GUEST_FUNCTION_HOOK(__imp__NetDll_ioctlsocket, NetDll_ioctlsocket);
GUEST_FUNCTION_HOOK(__imp__NetDll_sendto, NetDll_sendto);
GUEST_FUNCTION_HOOK(__imp__NetDll_recvfrom, NetDll_recvfrom);
GUEST_FUNCTION_HOOK(__imp__NetDll_shutdown, NetDll_shutdown);
GUEST_FUNCTION_HOOK(__imp__XMsgCancelIORequest, XMsgCancelIORequest);
GUEST_FUNCTION_HOOK(__imp__XAudioGetSpeakerConfig, XAudioGetSpeakerConfig);
GUEST_FUNCTION_HOOK(__imp__XamContentSetThumbnail, XamContentSetThumbnail);
GUEST_FUNCTION_HOOK(__imp__XamInputGetKeystrokeEx, XamInputGetKeystrokeEx);
GUEST_FUNCTION_HOOK(__imp__XamSessionCreateHandle, XamSessionCreateHandle);
GUEST_FUNCTION_HOOK(__imp__XamSessionRefObjByHandle, XamSessionRefObjByHandle);
GUEST_FUNCTION_HOOK(__imp__KeSetDisableBoostThread, KeSetDisableBoostThread);
GUEST_FUNCTION_HOOK(__imp__XamCreateEnumeratorHandle, XamCreateEnumeratorHandle);
GUEST_FUNCTION_HOOK(__imp__NtDeviceIoControlFile, NtDeviceIoControlFile);
GUEST_FUNCTION_HOOK(__imp__NetDll_WSAGetLastError, NetDll_WSAGetLastError);
GUEST_FUNCTION_HOOK(__imp__NetDll_XNetQosListen, NetDll_XNetQosListen);
GUEST_FUNCTION_HOOK(__imp__NetDll_XNetQosLookup, NetDll_XNetQosLookup);
GUEST_FUNCTION_HOOK(__imp__NetDll_XNetQosRelease, NetDll_XNetQosRelease);
GUEST_FUNCTION_HOOK(__imp__NetDll_XNetServerToInAddr, NetDll_XNetServerToInAddr);
GUEST_FUNCTION_HOOK(__imp__NetDll_XNetXnAddrToInAddr, NetDll_XNetXnAddrToInAddr);
GUEST_FUNCTION_HOOK(__imp__NetDll_XNetGetConnectStatus, NetDll_XNetGetConnectStatus);
GUEST_FUNCTION_HOOK(__imp__NetDll_XNetUnregisterInAddr, NetDll_XNetUnregisterInAddr);
