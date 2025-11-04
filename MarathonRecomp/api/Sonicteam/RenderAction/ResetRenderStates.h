#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class ResetRenderStates : public MyRenderProcess {};

    MARATHON_ASSERT_SIZEOF(ResetRenderStates, 0x38);
}
