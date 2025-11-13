#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetStencilEnable : public MyRenderProcess
    {
    public:
        bool m_FrontEnable;
        bool m_BackEnable;
    };

    MARATHON_ASSERT_OFFSETOF(SetStencilEnable, m_FrontEnable, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetStencilEnable, m_BackEnable, 0x39);
    MARATHON_ASSERT_SIZEOF(SetStencilEnable, 0x3C);
}
