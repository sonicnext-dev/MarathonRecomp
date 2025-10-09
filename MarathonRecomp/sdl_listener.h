#pragma once

class ISDLEventListener
{
public:
    virtual ~ISDLEventListener() = default;
    virtual bool OnSDLEvent(SDL_Event* event) = 0;
};

extern std::vector<ISDLEventListener*>& GetEventListeners();

class SDLEventListener : public ISDLEventListener
{
public:
    SDLEventListener()
    {
        GetEventListeners().emplace_back(this);
    }

    ~SDLEventListener() override
    {
        auto& eventListeners = GetEventListeners();

        auto it = std::find(eventListeners.begin(), eventListeners.end(), this);

        assert(it != eventListeners.end());

        eventListeners.erase(it);
    }

    bool OnSDLEvent(SDL_Event* event) override { return false; }
};
