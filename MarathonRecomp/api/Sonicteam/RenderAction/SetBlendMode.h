#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetBlendMode : public MyRenderProcess
    {
    public:
        enum BlendMode
        {
            BlendMode_Opaque = 0,
            BlendMode_Blend = 1,
            BlendMode_Add = 2
        };

        be<BlendMode> m_BlendMode;
    };

    MARATHON_ASSERT_OFFSETOF(SetBlendMode, m_BlendMode, 0x38);
    MARATHON_ASSERT_SIZEOF(SetBlendMode, 0x3C);
}
