#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/Camera/CameraMode.h>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::Camera::Cameraman
{
    struct MsgPushCameraMode : SoX::Message<0x14005>
    {
        boost::shared_ptr<Sonicteam::Camera::CameraMode> spCameraMode{};
        be<uint32_t> Field0C{};
        uint8_t Field10{};
        be<float> Field14{};
    };

    MARATHON_ASSERT_OFFSETOF(MsgPushCameraMode, spCameraMode, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgPushCameraMode, Field0C, 0x0C);
    MARATHON_ASSERT_OFFSETOF(MsgPushCameraMode, Field10, 0x10);
    MARATHON_ASSERT_OFFSETOF(MsgPushCameraMode, Field14, 0x14);
    MARATHON_ASSERT_SIZEOF(MsgPushCameraMode, 0x18);
}
