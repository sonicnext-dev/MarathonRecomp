#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class PrepareCalculateCSM : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x18);
    };

    MARATHON_ASSERT_SIZEOF(PrepareCalculateCSM, 0x50);
}
