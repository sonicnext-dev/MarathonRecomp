#pragma once

#include <functional>
#include <string>

typedef struct SDL_Thread SDL_Thread;

class HostThread
{
public:
    HostThread(const std::string& name, std::function<void(void*)> func, void* data);
    HostThread(const std::string& name, std::function<void()> func)
        : HostThread(name, [f = std::move(func)](void*) { f(); }, nullptr)
    {
    }

    ~HostThread();

    void Join();
    void Detach();

    uint32_t GetID() const;
    bool IsAlive() const;

    static uint32_t GetCurrentID();

private:
    static int HostThreadFunc(void* data);

    SDL_Thread* thread;

    std::function<void(void*)> func;
    void* data;
};