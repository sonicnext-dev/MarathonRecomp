#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetColorWriteEnable : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(SetColorWriteEnable, 0x3C);
}
