#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::Player
{
    struct MsgSuckPlayer : SoX::Message<0x11034A>
    {
        SoX::Math::Vector Point{};
    };

    MARATHON_ASSERT_OFFSETOF(MsgSuckPlayer, Point, 0x10);
}
