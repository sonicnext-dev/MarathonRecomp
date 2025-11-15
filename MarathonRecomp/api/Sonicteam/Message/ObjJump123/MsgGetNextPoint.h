#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::ObjJump123
{
    struct MsgGetNextPoint : SoX::Message<0x10007>
    {
        SoX::Math::Quaternion Rotation{};
        SoX::Math::Vector Position{};
    };

    MARATHON_ASSERT_OFFSETOF(MsgGetNextPoint, Rotation, 0x10);
    MARATHON_ASSERT_OFFSETOF(MsgGetNextPoint, Position, 0x20);
}
