#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::Camera::Cameraman
{
    struct MsgChangeMode : SoX::Message<0x14007>
    {
        be<uint32_t> PadID{};
        be<uint32_t> TargetActorID{};
        bool IsDemoCamera{};
    };

    MARATHON_ASSERT_OFFSETOF(MsgChangeMode, PadID, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgChangeMode, TargetActorID, 0x08);
    MARATHON_ASSERT_OFFSETOF(MsgChangeMode, IsDemoCamera, 0x0C);
}
