#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetScissorTest : public MyRenderProcess
    {
    public:
        bool m_Enabled;
    };

    MARATHON_ASSERT_SIZEOF(SetScissorTest, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetScissorTest, m_Enabled, 0x38);
}
