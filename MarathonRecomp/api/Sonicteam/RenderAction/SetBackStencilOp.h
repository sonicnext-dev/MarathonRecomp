#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetBackStencilOp : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0xC);
    };

    MARATHON_ASSERT_SIZEOF(SetBackStencilOp, 0x44);
}
