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
            Opaque = 0,
            Blend = 1,
            Add = 2
        };

        be<BlendMode> m_BlendMode;
    };

    MARATHON_ASSERT_SIZEOF(SetBlendMode, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetBlendMode, m_BlendMode, 0x38);
}
