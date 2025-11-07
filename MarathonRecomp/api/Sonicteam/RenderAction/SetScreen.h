#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetScreen : public MyRenderProcess
    {
    public:
        be<uint32_t> m_CameraIndex;
        be<float> m_Width;
        be<float> m_Height;
    };

    MARATHON_ASSERT_SIZEOF(SetScreen, 0x44);
    MARATHON_ASSERT_OFFSETOF(SetScreen, m_CameraIndex, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetScreen, m_Width, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetScreen, m_Height, 0x40);
}
