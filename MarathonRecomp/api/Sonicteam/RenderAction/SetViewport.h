#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetViewport : public MyRenderProcess
    {
    public:
        be<float> m_X;
        be<float> m_Y;
        be<float> m_Width;
        be<float> m_Height;
    };

    MARATHON_ASSERT_OFFSETOF(SetViewport, m_X, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetViewport, m_Y, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetViewport, m_Width, 0x40);
    MARATHON_ASSERT_OFFSETOF(SetViewport, m_Height, 0x44);
    MARATHON_ASSERT_SIZEOF(SetViewport, 0x48);
}
