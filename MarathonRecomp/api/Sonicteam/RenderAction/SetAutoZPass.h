#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetAutoZPass : public MyRenderProcess
    {
    public:
        bool m_Enabled;
    };

    MARATHON_ASSERT_SIZEOF(SetAutoZPass, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetAutoZPass, m_Enabled, 0x38);
}
