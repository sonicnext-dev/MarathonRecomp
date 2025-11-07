#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetDepthTextures : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x8);
    };

    MARATHON_ASSERT_SIZEOF(SetDepthTextures, 0x40);
}
