#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetClip : public MyRenderProcess
    {
    public:
        be<uint32_t> m_CameraIndex;
        MARATHON_INSERT_PADDING(0x8);
    };

    MARATHON_ASSERT_SIZEOF(SetClip, 0x44);
    MARATHON_ASSERT_OFFSETOF(SetClip, m_CameraIndex, 0x38);
}
