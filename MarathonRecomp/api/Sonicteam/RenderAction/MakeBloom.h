#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class MakeBloom : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x34);
    };

    MARATHON_ASSERT_SIZEOF(MakeBloom, 0x6C);
}
