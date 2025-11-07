#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetScreen : public MyRenderProcess
    {
    public:
        be<uint32_t> m_CameraIndex;
        MARATHON_INSERT_PADDING(0x8);
    };

    MARATHON_ASSERT_SIZEOF(SetScreen, 0x44);
    MARATHON_ASSERT_OFFSETOF(SetScreen, m_CameraIndex, 0x38);
}
