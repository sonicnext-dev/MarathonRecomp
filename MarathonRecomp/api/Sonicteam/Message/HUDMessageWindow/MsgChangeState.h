#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::HUDMessageWindow
{
    struct MsgChangeState : SoX::Message<0x1B051>
    {
        be<uint32_t> State{};
        MARATHON_INSERT_PADDING(0x04);

        MsgChangeState(uint32_t in_state) : State(in_state) {}
    };

    MARATHON_ASSERT_OFFSETOF(MsgChangeState, State, 0x04);
    MARATHON_ASSERT_SIZEOF(MsgChangeState, 0x0C);
}
