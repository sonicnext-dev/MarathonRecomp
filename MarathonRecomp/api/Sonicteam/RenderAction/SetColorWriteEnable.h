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
            ColorWrite_None = 0,
            ColorWrite_Red = 1 << 0,
            ColorWrite_Green = 1 << 1,
            ColorWrite_Blue = 1 << 2,
            ColorWrite_Alpha = 1 << 3
        };

        be<ColorWrite> m_ColorWriteMask;
    };

    MARATHON_ASSERT_OFFSETOF(SetColorWriteEnable, m_ColorWriteMask, 0x38);
    MARATHON_ASSERT_SIZEOF(SetColorWriteEnable, 0x3C);
}
