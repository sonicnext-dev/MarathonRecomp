#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam
{
    class RenderPostprocess : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(RenderPostprocess, 0x3C);
}
