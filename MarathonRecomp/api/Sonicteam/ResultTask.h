#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class ResultTask : public SoX::Engine::Task
    {
    public:
        be<uint32_t> m_State;
        be<float> m_Time;
        MARATHON_INSERT_PADDING(0xA4);
    };

    MARATHON_ASSERT_OFFSETOF(ResultTask, m_State, 0x4C);
    MARATHON_ASSERT_OFFSETOF(ResultTask, m_Time, 0x50);
    MARATHON_ASSERT_SIZEOF(ResultTask, 0xF8);
}
