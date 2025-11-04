#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetStencilOp : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x8);
    };

    MARATHON_INSERT_PADDING(0x44);
}
