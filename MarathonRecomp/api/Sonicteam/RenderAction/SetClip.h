#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetClip : public MyRenderProcess
    {
    public:
        be<uint32_t> m_CameraIndex;
        be<float> m_Clip1;
        be<float> m_Clip2;
    };

    MARATHON_ASSERT_SIZEOF(SetClip, 0x44);
    MARATHON_ASSERT_OFFSETOF(SetClip, m_CameraIndex, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetClip, m_Clip1, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetClip, m_Clip2, 0x40);
}
