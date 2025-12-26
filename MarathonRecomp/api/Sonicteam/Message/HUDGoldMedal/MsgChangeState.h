#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::HUDGoldMedal
{
    struct MsgChangeState : SoX::Message<0x1B058>
    {
        be<uint32_t> State{};
        be<uint32_t> Field08{};
        uint8_t Field0C{};
        be<uint32_t> EpisodeIndex{};

        MsgChangeState() {}

        MsgChangeState(uint32_t state, uint32_t episodeIndex = 0) : State(state), EpisodeIndex(episodeIndex) {}
    };

    MARATHON_ASSERT_OFFSETOF(MsgChangeState, State, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgChangeState, Field08, 0x08);
    MARATHON_ASSERT_OFFSETOF(MsgChangeState, Field0C, 0x0C);
    MARATHON_ASSERT_OFFSETOF(MsgChangeState, EpisodeIndex, 0x10);
}
