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
            PassIndex_Main,
            PassIndex_Transparent,
            PassIndex_Sky,
            PassIndex_Shadowmap,
            PassIndex_Psi,
            PassIndex_Oc,
            PassIndex_Glare,
            PassIndex_AfterPp,
            PassIndex_Radermap,
            PassIndex_User0,
            PassIndex_User1
        };

        be<PassIndex> m_PassIndex;
        xpointer<void> m_pShader;
    };

    MARATHON_ASSERT_OFFSETOF(SetConstantShader, m_PassIndex, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetConstantShader, m_pShader, 0x3C);
    MARATHON_ASSERT_SIZEOF(SetConstantShader, 0x40);
}
