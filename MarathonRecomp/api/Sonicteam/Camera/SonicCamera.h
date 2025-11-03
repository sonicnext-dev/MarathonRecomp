#pragma once

#include <Marathon.inl>

namespace Sonicteam::Camera
{
    class SonicCamera : public CameraMode
    {
    public:
        MARATHON_INSERT_PADDING(4);
        xpointer<DocMarathonState> m_pDoc;
        MARATHON_INSERT_PADDING(0x14);
        be<float> m_SpringK;
        be<float> m_DampingK;
        be<float> m_AzDamping;
        be<float> m_AltDamping;
        be<float> m_AzDriveK;
        be<float> m_AzDampingK;
        be<float> m_AltDriveK;
        be<float> m_AltDampingK;
        MARATHON_INSERT_PADDING(0x3C);
        be<float> m_FovY;
        MARATHON_INSERT_PADDING(0x3C);
        be<float> m_Distance;
        MARATHON_INSERT_PADDING(0x0C);
    };

    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_pDoc, 0x2C);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_SpringK, 0x44);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_DampingK, 0x48);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_AzDamping, 0x4C);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_AltDamping, 0x50);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_AzDriveK, 0x54);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_AzDampingK, 0x58);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_AltDriveK, 0x5C);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_AltDampingK, 0x60);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_FovY, 0xA0);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_Distance, 0xE0);
    MARATHON_ASSERT_SIZEOF(SonicCamera, 0xF0);
}
