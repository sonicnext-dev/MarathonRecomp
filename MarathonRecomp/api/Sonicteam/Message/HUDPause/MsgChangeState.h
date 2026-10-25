#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::HUDPause
{
    struct MsgChangeState : SoX::Message<0x1B04E>
    {
        be<uint32_t> State{};
        be<uint32_t> Field08{};

        MsgChangeState(const uint32_t in_state, const uint32_t in_field08)
            : State(in_state), Field08(in_field08) {}
    };

    MARATHON_ASSERT_OFFSETOF(MsgChangeState, State, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgChangeState, Field08, 0x08);
    MARATHON_ASSERT_SIZEOF(MsgChangeState, 0x0C);
}
