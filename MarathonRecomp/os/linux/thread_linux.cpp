#include <os/thread.h>
#include <pthread.h>

bool os::thread::SetCurrentThreadName(const char* name)
{
    // Linux limits thread names to 15 characters, and will
    // reject any attempt to set a longer name with ERANGE.
    const auto truncated = std::string(name, std::min(strlen(name), size_t(15)));

    return pthread_setname_np(pthread_self(), truncated.c_str()) == 0;
}
