#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>
#include <stdx/string.h>

namespace Sonicteam::Message::PauseAdapter
{
    struct MsgGetText : SoX::Message<0x1C003>
    {
        stdx::string PauseName{};
        stdx::string SelectedName{};
    };

    MARATHON_ASSERT_OFFSETOF(MsgGetText, PauseName, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgGetText, SelectedName, 0x20);
}
