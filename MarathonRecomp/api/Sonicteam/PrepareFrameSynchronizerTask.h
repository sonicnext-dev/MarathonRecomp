#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class PrepareFrameSynchronizerTask : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(PrepareFrameSynchronizerTask, 0x50);
}
