#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class ApplyDevice : public MyRenderProcess {};

    MARATHON_ASSERT_SIZEOF(ApplyDevice, 0x38);
}
