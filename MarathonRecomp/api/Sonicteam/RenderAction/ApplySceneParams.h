#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class ApplySceneParams : public MyRenderProcess {};

    MARATHON_ASSERT_SIZEOF(ApplySceneParams, 0x38);
}
