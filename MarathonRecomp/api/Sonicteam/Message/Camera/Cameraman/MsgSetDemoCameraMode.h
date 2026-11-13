#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::Camera::Cameraman
{
    struct MsgSetDemoCameraMode : SoX::Message<0x14007>
    {
        be<uint32_t> PadID{};
        be<uint32_t> TargetActorID{};
        bool Enabled{};
    };

    MARATHON_ASSERT_OFFSETOF(MsgSetDemoCameraMode, PadID, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgSetDemoCameraMode, TargetActorID, 0x08);
    MARATHON_ASSERT_OFFSETOF(MsgSetDemoCameraMode, Enabled, 0x0C);
    MARATHON_ASSERT_SIZEOF(MsgSetDemoCameraMode, 0x10);
}
