#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetFovY : public MyRenderProcess
    {
    public:
        be<uint32_t> m_CameraIndex;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(SetFovY, 0x40);
}
