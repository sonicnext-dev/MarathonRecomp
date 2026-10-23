#pragma once

#include <Marathon.inl>
#include <Sonicteam/Camera/Cameraman.h>
#include <Sonicteam/Camera/CameraMode.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Camera
{
    class DemoGMCamera : public CameraMode
    {
    public:
        xpointer<Cameraman> m_pCameraman;
        be<uint32_t> m_TargetActorID;
        be<float> m_Distance;
        be<float> m_DefaultDistance;
        SoX::Math::Vector3 m_Speed;
        be<float> m_Pitch;
        be<float> m_Yaw;
        SoX::Math::Vector m_Position;
        SoX::Math::Vector m_Target;
        SoX::Math::Quaternion m_Rotation;
    };

    MARATHON_ASSERT_OFFSETOF(DemoGMCamera, m_pCameraman, 0x28);
    MARATHON_ASSERT_OFFSETOF(DemoGMCamera, m_TargetActorID, 0x2C);
    MARATHON_ASSERT_OFFSETOF(DemoGMCamera, m_Distance, 0x30);
    MARATHON_ASSERT_OFFSETOF(DemoGMCamera, m_DefaultDistance, 0x34);
    MARATHON_ASSERT_OFFSETOF(DemoGMCamera, m_Speed, 0x38);
    MARATHON_ASSERT_OFFSETOF(DemoGMCamera, m_Pitch, 0x44);
    MARATHON_ASSERT_OFFSETOF(DemoGMCamera, m_Yaw, 0x48);
    MARATHON_ASSERT_OFFSETOF(DemoGMCamera, m_Position, 0x50);
    MARATHON_ASSERT_OFFSETOF(DemoGMCamera, m_Target, 0x60);
    MARATHON_ASSERT_OFFSETOF(DemoGMCamera, m_Rotation, 0x70);
    MARATHON_ASSERT_SIZEOF(DemoGMCamera, 0x80);
}
