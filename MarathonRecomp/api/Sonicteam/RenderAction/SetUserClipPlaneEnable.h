#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetUserClipPlaneEnable : public MyRenderProcess
    {
    public:
        be<uint32_t> m_CameraIndex;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(SetUserClipPlaneEnable, 0x40);
    MARATHON_ASSERT_OFFSETOF(SetUserClipPlaneEnable, m_CameraIndex, 0x38);
}
