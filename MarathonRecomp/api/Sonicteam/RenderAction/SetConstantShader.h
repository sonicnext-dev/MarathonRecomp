#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetConstantShader : public MyRenderProcess
    {
    public:
        enum PassIndex
        {
            Main,
            Transparent,
            Sky,
            Shadowmap,
            Psi,
            Oc,
            Glare,
            AfterPp,
            Radermap,
            User0,
            User1
        };

        be<PassIndex> m_PassIndex;
        xpointer<void> m_pShader;
    };

    MARATHON_ASSERT_SIZEOF(SetConstantShader, 0x40);
    MARATHON_ASSERT_OFFSETOF(SetConstantShader, m_PassIndex, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetConstantShader, m_pShader, 0x3C);
}
