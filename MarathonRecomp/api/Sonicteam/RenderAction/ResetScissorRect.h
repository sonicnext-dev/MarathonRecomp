#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class ResetScissorRect : public MyRenderProcess {};

    MARATHON_ASSERT_SIZEOF(ResetScissorRect, 0x38);
}
