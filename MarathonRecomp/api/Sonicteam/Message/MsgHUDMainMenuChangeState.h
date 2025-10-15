#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message
{
    struct MsgHUDMainMenuChangeState : SoX::Message<0x1B053>
    {
        be<uint32_t> State;
        be<uint32_t> CursorIndex;

        MsgHUDMainMenuChangeState() {}

        MsgHUDMainMenuChangeState(uint32_t state) : State(state) {}

        MsgHUDMainMenuChangeState(uint32_t state, uint32_t cursorIndex)
            : State(state), CursorIndex(cursorIndex) {}
    };
}
