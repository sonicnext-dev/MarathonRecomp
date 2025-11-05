#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message
{
    struct MsgObjJump123GetNextPoint : SoX::Message<0x10007>
    {
        SoX::Math::Quaternion Rotation;
		SoX::Math::Vector Position;
    };

    MARATHON_ASSERT_OFFSETOF(MsgObjJump123GetNextPoint, Rotation, 0x10);
    MARATHON_ASSERT_OFFSETOF(MsgObjJump123GetNextPoint, Position, 0x20);
}
