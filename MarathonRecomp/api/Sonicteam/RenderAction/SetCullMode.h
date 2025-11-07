#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetCullMode : public MyRenderProcess
    {
    public:
        enum CullMode
        {
            None,
            CW,
            CCW
        };

        be<CullMode> m_CullMode;
    };

    MARATHON_ASSERT_OFFSETOF(SetCullMode, m_CullMode, 0x38);
    MARATHON_ASSERT_SIZEOF(SetCullMode, 0x3C);
}
