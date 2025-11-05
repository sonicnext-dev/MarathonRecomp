#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class PauseTask : public SoX::Engine::Task
    {
    public:
        enum PauseTaskState
        {
            PauseTaskState_Opened,
            PauseTaskState_Opening,
            PauseTaskState_Idle,
            PauseTaskState_ClosingToAction,
            PauseTaskState_Closing,
            PauseTaskState_Closed
        };

        MARATHON_INSERT_PADDING(4);
        xpointer<void> m_pHUDPause;
        MARATHON_INSERT_PADDING(0x18);
        be<PauseTaskState> m_State;
        be<uint32_t> m_Flags;
        be<uint32_t> m_SelectedIndex;
        MARATHON_INSERT_PADDING(0x28);
        be<uint32_t> m_Buttons;
        MARATHON_INSERT_PADDING(0x1E0);
        be<uint32_t> m_ItemCount;
        MARATHON_INSERT_PADDING(0x12C);
    };

    MARATHON_ASSERT_OFFSETOF(PauseTask, m_pHUDPause, 0x50);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_State, 0x6C);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_Flags, 0x70);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_SelectedIndex, 0x74);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_Buttons, 0xA0);
    MARATHON_ASSERT_OFFSETOF(PauseTask, m_ItemCount, 0x284);
    MARATHON_ASSERT_SIZEOF(PauseTask, 0x3B4);
}
