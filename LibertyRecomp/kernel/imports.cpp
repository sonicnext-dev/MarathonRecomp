#include <cstdint>
#include <cstdio>
#include <stdafx.h>
#include <cpu/ppc_context.h>
#include <cpu/guest_thread.h>
#include <apu/audio.h>
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

#ifdef _WIN32
#include <ntstatus.h>
#endif

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

    struct NtFileHandle final : KernelObject
    {
        uint32_t magic = kNtFileHandleMagic;
        std::fstream stream;
        std::filesystem::path path;
    };

    struct NtDirHandle final : KernelObject
    {
        uint32_t magic = kNtDirHandleMagic;
        std::filesystem::path path;
        std::vector<std::filesystem::directory_entry> entries;
        size_t cursor = 0;
        bool initialized = false;
    };

    static std::unordered_map<uint32_t, NtFileHandle*> g_ntFileHandles;
    static std::unordered_map<uint32_t, NtDirHandle*> g_ntDirHandles;

    static uint32_t AlignUp(uint32_t value, uint32_t align)
    {
        return (value + (align - 1)) & ~(align - 1);
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
            // Allow empty paths to pass through, they'll fail later
        }
        else if (len > 1024)
        {
            LOGF_IMPL(Utility, "TryGetAnsiPath", "Suspicious length: {} (max 1024)", len);
            return false;
        }
        else
        {
            // Check if the path looks valid (should contain printable ASCII)
            bool hasValid = false;
            bool hasGarbage = false;
            for (size_t i = 0; i < std::min<size_t>(len, 20); i++)
            {
                unsigned char c = static_cast<unsigned char>(buf[i]);
                if (c >= 32 && c <= 126)
                    hasValid = true;
                else if (c >= 128)
                    hasGarbage = true;
            }
            if (hasGarbage && !hasValid)
            {
                LOGF_IMPL(Utility, "TryGetAnsiPath", "Garbage path detected (len={}, first byte=0x{:02X})", len, static_cast<unsigned char>(buf[0]));
                // Still allow it to pass for now, so we can see the full picture
            }
        }

        // Log the raw pointer values for debugging
        // LOGF_IMPL(Utility, "TryGetAnsiPath", "Name.ptr=0x{:08X} Length={} Buffer.ptr=0x{:08X}", 
        //     attributes->Name.ptr.value, len, name->Buffer.ptr.value);

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
    LOG_UTILITY("!!! STUB !!!");
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

    // Check for directory
    {
        std::error_code ec;
        bool exists = std::filesystem::exists(resolved, ec);
        bool isDir = exists && std::filesystem::is_directory(resolved, ec);
        
        if (exists && isDir && !ec)
        {
            NtDirHandle* hDir = CreateKernelObject<NtDirHandle>();
            hDir->path = resolved;

            const uint32_t handleValue = GetKernelHandle(hDir);
            g_ntDirHandles.emplace(handleValue, hDir);

            *FileHandle = handleValue;
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

    const uint32_t handleValue = GetKernelHandle(h);
    g_ntFileHandles.emplace(handleValue, h);

    *FileHandle = handleValue;
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
    LOGF_IMPL(Utility, "NtCreateFile", "Guest: '{}' -> Resolved: '{}'", guestPath, resolved.string());

    // Directories can't be opened with std::fstream; represent them with a dedicated handle.
    {
        std::error_code ec;
        bool exists = std::filesystem::exists(resolved, ec);
        bool isDir = exists && std::filesystem::is_directory(resolved, ec);
        LOGF_IMPL(Utility, "NtCreateFile", "exists={} isDir={} ec={}", exists, isDir, ec ? ec.message() : "none");
        
        if (exists && isDir && !ec)
        {
            NtDirHandle* hDir = CreateKernelObject<NtDirHandle>();
            hDir->path = resolved;

            const uint32_t handleValue = GetKernelHandle(hDir);
            g_ntDirHandles.emplace(handleValue, hDir);

            *FileHandle = handleValue;
            LOGF_IMPL(Utility, "NtCreateFile", "Created directory handle 0x{:08X} for {}", handleValue, resolved.string());
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
        LOGF_IMPL(Utility, "NtCreateFile", "fstream.open failed: errno={} status=0x{:08X}", errno, status);
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
    if (auto it = g_ntFileHandles.find(FileHandle); it != g_ntFileHandles.end() && it->second && it->second->magic == kNtFileHandleMagic)
    {
        hFile = it->second;
        path = hFile->path;
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
    const bool exists = std::filesystem::exists(path, ec);
    if (!exists || ec)
    {
        IoStatusBlock->Status = STATUS_OBJECT_NAME_NOT_FOUND;
        IoStatusBlock->Information = 0;
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    const bool isDir = std::filesystem::is_directory(path, ec);
    if (ec)
    {
        IoStatusBlock->Status = STATUS_FAIL_CHECK;
        IoStatusBlock->Information = 0;
        return STATUS_FAIL_CHECK;
    }

    uint64_t fileSize = 0;
    if (!isDir)
    {
        fileSize = std::filesystem::file_size(path, ec);
        if (ec)
            fileSize = 0;
    }

    const uint64_t allocationSize = RoundUpToPage(fileSize);
    const uint32_t attrs = GetFileAttributesBestEffort(path);

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
    default:
        LOGF_WARNING("NtQueryInformationFile: unhandled class {}", FileInformationClass);
        IoStatusBlock->Status = STATUS_INVALID_PARAMETER;
        IoStatusBlock->Information = 0;
        return STATUS_INVALID_PARAMETER;
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
    LOGF_IMPL(Utility, "NtQueryDirectoryFile", "Handle=0x{:08X}", FileHandle);
    if (FileHandle == GUEST_INVALID_HANDLE_VALUE || !IsKernelObject(FileHandle))
    {
        LOG_IMPL(Utility, "NtQueryDirectoryFile", "INVALID_HANDLE_VALUE or not kernel object");
        return STATUS_INVALID_HANDLE;
    }

    auto it = g_ntDirHandles.find(FileHandle);
    if (it == g_ntDirHandles.end() || !it->second || it->second->magic != kNtDirHandleMagic)
    {
        LOGF_IMPL(Utility, "NtQueryDirectoryFile", "Handle not in g_ntDirHandles (found={}, hasSecond={}, magic=0x{:08X})",
            it != g_ntDirHandles.end(), it != g_ntDirHandles.end() && it->second != nullptr,
            (it != g_ntDirHandles.end() && it->second) ? it->second->magic : 0);
        return STATUS_INVALID_HANDLE;
    }

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
    if (FileHandle == GUEST_INVALID_HANDLE_VALUE || !IsKernelObject(FileHandle))
    {
        LOGF_IMPL(Utility, "NtReadFile", "INVALID handle 0x{:08X}", FileHandle);
        return STATUS_INVALID_HANDLE;
    }

    // Check if this is a directory handle - directories can't be read as files, 
    // but return END_OF_FILE instead of INVALID_HANDLE to be more permissive.
    if (auto dit = g_ntDirHandles.find(FileHandle); dit != g_ntDirHandles.end() && dit->second && dit->second->magic == kNtDirHandleMagic)
    {
        LOGF_IMPL(Utility, "NtReadFile", "Directory handle 0x{:08X} - returning END_OF_FILE", FileHandle);
        if (IoStatusBlock)
        {
            IoStatusBlock->Status = STATUS_END_OF_FILE;
            IoStatusBlock->Information = 0;
        }
        return STATUS_END_OF_FILE;
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

    if (ByteOffset != nullptr)
    {
        const int64_t offset = ByteOffset->get();
        hFile->stream.clear();
        hFile->stream.seekg(offset, std::ios::beg);
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

void ObDereferenceObject()
{
    LOG_UTILITY("!!! STUB !!!");
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
    LOG_UTILITY("!!! STUB !!!");
}

void VdGetSystemCommandBuffer()
{
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

void VdEnableRingBufferRPtrWriteBack()
{
    LOG_UTILITY("!!! STUB !!!");
}

void VdInitializeRingBuffer()
{
    LOG_UTILITY("!!! STUB !!!");
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

void VdSetGraphicsInterruptCallback()
{
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t VdInitializeEngines()
{
    LOG_UTILITY("!!! STUB !!!");
    return 1;
}

void VdIsHSIOTrainingSucceeded()
{
    LOG_UTILITY("!!! STUB !!!");
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
    LOG_UTILITY("!!! STUB !!!");
}

uint32_t VdRetrainEDRAM()
{
    return 0;
}

void VdRetrainEDRAMWorker()
{
    LOG_UTILITY("!!! STUB !!!");
}

void KeEnterCriticalRegion()
{
    LOG_UTILITY("!!! STUB !!!");
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
    LOGF_UTILITY("0x{:x}, 0x{:x}, 0x{:x}, 0x{:x}, 0x{:x}, 0x{:x}", flags, size, protect, minAddress, maxAddress, alignment);
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

uint32_t KeWaitForSingleObject(XDISPATCHER_HEADER* Object, uint32_t WaitReason, uint32_t WaitMode, bool Alertable, be<int64_t>* Timeout)
{
    const uint32_t timeout = GuestTimeoutToMilliseconds(Timeout);
    assert(timeout == INFINITE);

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
            assert(false && "Unrecognized kernel object type.");
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

void MmQueryAllocationSize()
{
    LOG_UTILITY("!!! STUB !!!");
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

void ExTerminateThread()
{
    LOG_UTILITY("!!! STUB !!!");
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
    LOG_UTILITY("!!! STUB !!!");
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
