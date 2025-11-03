#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message
{
    struct MsgCameramanChangeMode : SoX::Message<0x14007>
    {
        be<uint32_t> ControllerIndex;
        be<uint32_t> TargetActorID;
        bool IsDemoCamera;
    };

    MARATHON_ASSERT_OFFSETOF(MsgCameramanChangeMode, ControllerIndex, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgCameramanChangeMode, TargetActorID, 0x08);
    MARATHON_ASSERT_OFFSETOF(MsgCameramanChangeMode, IsDemoCamera, 0x0C);
}
