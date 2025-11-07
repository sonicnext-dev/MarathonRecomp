#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetColorWriteEnable : public MyRenderProcess
    {
    public:
        enum ColorWrite : uint32_t
        {
            None = 0,
            Red = 1 << 0,
            Green = 1 << 1,
            Blue = 1 << 2,
            Alpha = 1 << 3
        };

        be<ColorWrite> m_ColorWriteMask;
    };

    MARATHON_ASSERT_SIZEOF(SetColorWriteEnable, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetColorWriteEnable, m_ColorWriteMask, 0x38);
}
