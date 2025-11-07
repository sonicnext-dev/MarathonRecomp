#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetScissorRect : public MyRenderProcess
    {
    public:
        be<float> m_X;
        be<float> m_Y;
        be<float> m_Width;
        be<float> m_Height;
    };

    MARATHON_ASSERT_SIZEOF(SetScissorRect, 0x48);
    MARATHON_ASSERT_OFFSETOF(SetScissorRect, m_X, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetScissorRect, m_Y, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetScissorRect, m_Width, 0x40);
    MARATHON_ASSERT_OFFSETOF(SetScissorRect, m_Height, 0x44);
}
