#pragma once

namespace Sonicteam::Camera
{
    class SonicCamera : public CameraMode
    {
    public:
        MARATHON_INSERT_PADDING(0x1C);
        be<float> m_SpringK;
        be<float> m_DampingK;
        MARATHON_INSERT_PADDING(0x08);
        be<float> m_AzDriveK;
        be<float> m_AzDampingK;
        be<float> m_AltDriveK;
        be<float> m_AltDampingK;
        MARATHON_INSERT_PADDING(0x3C);
        be<float> m_FovY;
    };
}
