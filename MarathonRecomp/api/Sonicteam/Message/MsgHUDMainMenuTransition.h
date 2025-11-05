#pragma once

#include <Marathon.inl>
#include <Sonicteam/Message/MsgHUDMainMenuChangeState.h>

namespace Sonicteam::Message
{
    struct MsgHUDMainMenuTransition : MsgHUDMainMenuChangeState
    {
        be<uint32_t> Flags;

        MsgHUDMainMenuTransition() {}

        MsgHUDMainMenuTransition(uint32_t state)
        {
            State = state;
        }

        MsgHUDMainMenuTransition(uint32_t state, uint32_t flags)
        {
            State = state;
            Flags = flags;
        }
    };

    MARATHON_ASSERT_OFFSETOF(MsgHUDMainMenuTransition, Flags, 0x08);
}
