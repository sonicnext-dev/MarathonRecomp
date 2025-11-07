#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class ResetViewport : public MyRenderProcess {};

    MARATHON_ASSERT_SIZEOF(ResetViewport, 0x38);
}
