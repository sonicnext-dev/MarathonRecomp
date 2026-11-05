#include "host_thread.h"
#include <os/logger.h>

#include <utility>

int HostThread::HostThreadFunc(void* data)
{
    const auto* thread = static_cast<HostThread*>(data);
    thread->func(thread->data);
    return 0;
}

HostThread::HostThread(const std::string& name, std::function<void(void*)> func, void* data)
    : func(std::move(func)), data(data)
{
    SDL_PropertiesID props = SDL_CreateProperties();
    if (props == 0) {
        LOGF_ERROR("SDL_CreateProperties failed: {}\n", SDL_GetError());
        return;
    }

    SDL_SetPointerProperty(props, SDL_PROP_THREAD_CREATE_ENTRY_FUNCTION_POINTER, reinterpret_cast<void*>(HostThreadFunc));
    SDL_SetPointerProperty(props, SDL_PROP_THREAD_CREATE_USERDATA_POINTER, this);
    SDL_SetNumberProperty(props, SDL_PROP_THREAD_CREATE_STACKSIZE_NUMBER, 1 * 1024 * 1024);
    SDL_SetStringProperty(props, SDL_PROP_THREAD_CREATE_NAME_STRING, name.c_str());

    thread = SDL_CreateThreadWithProperties(props);
    if (thread == nullptr) {
        LOGF_ERROR("SDL_CreateThreadWithProperties failed: {}\n", SDL_GetError());
        return;
    }

    SDL_DestroyProperties(props);
}

HostThread::~HostThread()
{
    Join();
}

void HostThread::Join()
{
    if (SDL_GetThreadState(thread) != SDL_THREAD_DETACHED)
        SDL_WaitThread(thread, nullptr);
}

void HostThread::Detach()
{
    SDL_DetachThread(thread);
}

uint32_t HostThread::GetID() const
{
    return SDL_GetThreadID(thread);
}

bool HostThread::IsAlive() const
{
    return SDL_GetThreadState(thread) == SDL_THREAD_ALIVE;
}

uint32_t HostThread::GetCurrentID() {
    return SDL_GetCurrentThreadID();
}
