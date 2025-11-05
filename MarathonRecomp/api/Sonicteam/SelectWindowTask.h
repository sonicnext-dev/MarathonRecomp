#pragma once

#include <Marathon.inl>
#include <Sonicteam/HUDCALLBACK.h>

namespace Sonicteam
{
    class SelectWindowTask : public SoX::Engine::Task, public HUDCALLBACK
    {
    public:
        MARATHON_INSERT_PADDING(0x40);
        be<uint32_t> m_ChosenIndex;
        MARATHON_INSERT_PADDING(0x28);
        be<uint32_t> m_SelectedIndex;
        MARATHON_INSERT_PADDING(0x14);
        be<uint32_t> m_BreatheFrame;
        MARATHON_INSERT_PADDING(0x28);
    };

    MARATHON_ASSERT_OFFSETOF(SelectWindowTask, m_ChosenIndex, 0x90);
    MARATHON_ASSERT_OFFSETOF(SelectWindowTask, m_SelectedIndex, 0xBC);
    MARATHON_ASSERT_OFFSETOF(SelectWindowTask, m_BreatheFrame, 0xD4);
    MARATHON_ASSERT_SIZEOF(SelectWindowTask, 0x100);
}
