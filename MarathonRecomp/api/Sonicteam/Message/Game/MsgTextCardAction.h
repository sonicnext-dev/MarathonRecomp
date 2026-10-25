#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>
#include <stdx/string.h>
#include <stdx/vector.h>

namespace Sonicteam::Message::Game
{
    struct MsgTextCardAction : SoX::Message<0x1D010>
    {
        stdx::string Name{};
        stdx::vector<stdx::string> Parameters{};
    };

    MARATHON_ASSERT_OFFSETOF(MsgTextCardAction, Name, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgTextCardAction, Parameters, 0x20);
    MARATHON_ASSERT_SIZEOF(MsgTextCardAction, 0x30);
}
