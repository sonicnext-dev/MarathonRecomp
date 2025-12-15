#pragma once

#include <filesystem>
#include <fstream>

// GTA IV Save Data Structure
struct GTA4SaveHeader
{
    static constexpr uint32_t SIGNATURE = 0x47544134; // "GTA4"
    static constexpr uint32_t VERSION = 1;
    
    uint32_t signature = SIGNATURE;
    uint32_t version = VERSION;
    uint32_t checksum = 0;
    uint32_t saveSlot = 0;
    
    bool VerifySignature() const { return signature == SIGNATURE; }
    bool VerifyVersion() const { return version <= VERSION; }
};

// Persistent data for the recomp itself (settings, DLC flags, etc.)
struct PersistentData
{
    static constexpr uint32_t SIGNATURE = 0x4C494234; // "LIB4" (Liberty IV)
    static constexpr uint32_t VERSION = 1;
    
    uint32_t Signature = SIGNATURE;
    uint32_t Version = VERSION;
    
    // DLC installation flags
    bool DLCFlags[8] = {};
    
    // First run flag
    bool HasShownWelcome = false;
    
    // Reserved for future use
    uint8_t Reserved[256] = {};
    
    bool VerifySignature() const { return Signature == SIGNATURE; }
    bool VerifyVersion() const { return Version <= VERSION; }
};

enum class EExtBinStatus
{
    Success,
    BadFileSize,
    BadSignature,
    BadVersion,
    IOError
};

class PersistentStorageManager
{
public:
    static inline PersistentData Data{};
    static inline EExtBinStatus BinStatus = EExtBinStatus::Success;
    
    static bool LoadBinary();
    static bool SaveBinary();
    static std::filesystem::path GetDataPath(bool extended);
    
    // GTA IV save file management
    static bool LoadGTA4Save(int slot, void* buffer, size_t size);
    static bool SaveGTA4Save(int slot, const void* buffer, size_t size);
    static bool DeleteGTA4Save(int slot);
    static bool GTA4SaveExists(int slot);
    static std::filesystem::path GetGTA4SavePath(int slot);
};
