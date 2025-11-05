#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message
{
    struct MsgHUDMainMenuChangeState : SoX::Message<0x1B053>
    {
        be<uint32_t> State;
    };

    MARATHON_ASSERT_OFFSETOF(MsgHUDMainMenuChangeState, State, 0x04);
}
