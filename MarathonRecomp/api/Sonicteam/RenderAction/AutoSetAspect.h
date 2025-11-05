#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class ApplyAutoAspect : public MyRenderProcess
    {
    public:
        be<uint32_t> m_CameraIndex;
    };

    MARATHON_ASSERT_SIZEOF(ApplyAutoAspect, 0x3C);
}
