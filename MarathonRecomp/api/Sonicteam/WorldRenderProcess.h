#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam
{
    class WorldRenderProcess : public MyRenderProcess
    {
    public:
        be<uint32_t> m_PassIndex;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(WorldRenderProcess, 0x40);
}