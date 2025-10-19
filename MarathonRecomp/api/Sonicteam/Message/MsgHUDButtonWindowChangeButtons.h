#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message
{
    struct MsgHUDButtonWindowChangeButtons : SoX::Message<0x1B05B>
    {
        be<uint32_t> ButtonType;

        MsgHUDButtonWindowChangeButtons(uint32_t buttonType = 0) : ButtonType(buttonType) {}
    };
}
