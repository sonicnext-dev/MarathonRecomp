#pragma once

#include <Marathon.inl>
#include <Sonicteam/Message/HUDMainMenu/MsgChangeState.h>

namespace Sonicteam::Message::HUDMainMenu
{
    struct MsgTransition : MsgChangeState
    {
        be<uint32_t> Flags{};

        MsgTransition() {}

        MsgTransition(uint32_t state)
        {
            State = state;
        }

        MsgTransition(uint32_t state, uint32_t flags)
        {
            State = state;
            Flags = flags;
        }
    };

    MARATHON_ASSERT_OFFSETOF(MsgTransition, Flags, 0x08);
}
