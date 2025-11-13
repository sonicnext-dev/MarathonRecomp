#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class RunCommandBuffer : public MyRenderProcess
    {
    public:
        be<uint32_t> m_CameraIndex;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_OFFSETOF(RunCommandBuffer, m_CameraIndex, 0x38);
    MARATHON_ASSERT_SIZEOF(RunCommandBuffer, 0x40);
}
