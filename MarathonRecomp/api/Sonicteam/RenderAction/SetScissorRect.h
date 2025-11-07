#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetScissorRect : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_SIZEOF(SetScissorRect, 0x48);
}
