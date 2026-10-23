#pragma once

#include <Marathon.inl>
#include <Sonicteam/Camera/CameraMode.h>
#include <Sonicteam/Player/Object.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/RefLinkNode.h>

namespace Sonicteam::Camera
{
    class SnipeCamera : public CameraMode
    {
    public:
        SoX::RefLinkNode<Player::Object> m_rPlayer;
        SoX::Math::Vector m_Field40;
        SoX::Math::Vector m_Field50;
        be<float> m_Pitch;
        be<float> m_Yaw;
        be<float> m_PitchAcceleration;
    };

    MARATHON_ASSERT_OFFSETOF(SnipeCamera, m_rPlayer, 0x28);
    MARATHON_ASSERT_OFFSETOF(SnipeCamera, m_Field40, 0x40);
    MARATHON_ASSERT_OFFSETOF(SnipeCamera, m_Field50, 0x50);
    MARATHON_ASSERT_OFFSETOF(SnipeCamera, m_Pitch, 0x60);
    MARATHON_ASSERT_OFFSETOF(SnipeCamera, m_Yaw, 0x64);
    MARATHON_ASSERT_OFFSETOF(SnipeCamera, m_PitchAcceleration, 0x68);
    MARATHON_ASSERT_SIZEOF(SnipeCamera, 0x70);
}
