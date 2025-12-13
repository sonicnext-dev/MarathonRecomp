#include <os/logger.h>

#include <cstdio>

void os::logger::Init()
{
}

void os::logger::Log(const std::string_view str, ELogType type, const char* func)
{
    if (func)
    {
        fmt::println("[{}] {}", func, str);
    }
    else
    {
        fmt::println("{}", str);
    }

    // On macOS, this app frequently exits via std::_Exit(), which bypasses
    // normal stdio flushing. Flushing here ensures logs are visible when
    // stdout is not a TTY (e.g. piped through a test harness).
    std::fflush(stdout);
}
