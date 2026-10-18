#pragma once

#include <message.h>
#include <SDL3/SDL.h>

namespace HID
{
    class MsgSDLEvent : public Message<MsgSDLEvent>
    {
    public:
        const SDL_Event* pEvent{};

        MsgSDLEvent(const SDL_Event* in_pEvent) : pEvent(in_pEvent) {}
    };
}
