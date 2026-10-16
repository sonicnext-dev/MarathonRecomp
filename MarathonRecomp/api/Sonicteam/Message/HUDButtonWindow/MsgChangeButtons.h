#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::HUDButtonWindow
{
    struct MsgChangeButtons : SoX::Message<0x1B05B>
    {
        be<uint32_t> ButtonType{};

        MsgChangeButtons(uint32_t buttonType = 0) : ButtonType(buttonType) {}
    };

    MARATHON_ASSERT_OFFSETOF(MsgChangeButtons, ButtonType, 0x04);
}
