#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetTexture : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x14);
    };

    MARATHON_ASSERT_SIZEOF(SetTexture, 0x4C);
}
