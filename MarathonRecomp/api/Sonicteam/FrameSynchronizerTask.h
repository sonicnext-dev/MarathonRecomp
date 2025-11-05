#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class FrameSynchronizerTask : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x2C);
    };

    MARATHON_ASSERT_SIZEOF(FrameSynchronizerTask, 0x78);
}
