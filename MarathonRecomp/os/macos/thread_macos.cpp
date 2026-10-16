#include <os/thread.h>
#include <pthread.h>

bool os::thread::SetCurrentThreadName(const char* name)
{
    // macOS truncates the thread name to 64 characters internally.
    return pthread_setname_np(name) == 0;
}
