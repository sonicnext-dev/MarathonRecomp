#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetConstantShader : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x8);
    };

    MARATHON_ASSERT_SIZEOF(SetConstantShader, 0x40);
}
