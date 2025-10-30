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
}
