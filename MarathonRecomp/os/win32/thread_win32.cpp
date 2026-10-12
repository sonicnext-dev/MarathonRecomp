#include <os/thread.h>
#include <os/version.h>

// Reference: https://learn.microsoft.com/visualstudio/debugger/how-to-set-a-thread-name-in-native-code

void _SetCurrentThreadNameLegacy(const char* name)
{
#pragma pack(push, 8)
    struct THREADNAME_INFO
    {
        DWORD dwType{ 0x1000 };
        LPCSTR szName{};
        DWORD dwThreadID{};
        DWORD dwFlags{};
    };
#pragma pack(pop)

    THREADNAME_INFO info
    {
        .szName = name,
        .dwThreadID = GetCurrentThreadId()
    };

    __try
    {
        RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { }
}

bool os::thread::SetCurrentThreadName(const char* name)
{
    const auto osVersion = os::version::GetOSVersion();

    if (osVersion.Major > 10 || (osVersion.Major == 10 && osVersion.Build >= 14393))
    {
        // I doubt we'll ever need Unicode in thread names.
        const auto nameView = std::string_view(name);
        const auto nameW = std::wstring(nameView.begin(), nameView.end());

        // Modern thread naming for Windows 10 1607 or later.
        // This method allows for thread names to be written to crash dumps.
        return !FAILED(SetThreadDescription(GetCurrentThread(), nameW.c_str()));
    }
    else
    {
        // Legacy thread naming for Windows 10 1511 or earlier.
        _SetCurrentThreadNameLegacy(name);
    }

    return true;
}
