#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::HUDLoading
{
    enum HUDLoadingState : uint32_t
    {
        HUDLoadingState_Open,
        HUDLoadingState_1,
        HUDLoadingState_Loop,
        HUDLoadingState_End
    };

    struct MsgChangeState : SoX::Message<0x1B03F>
    {
        be<HUDLoadingState> State{};
    };

    MARATHON_ASSERT_OFFSETOF(MsgChangeState, State, 0x04);
}
