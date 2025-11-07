#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class ColorFill : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_SIZEOF(ColorFill, 0x48);
}
