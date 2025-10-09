#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class PauseTask : public SoX::Engine::Task
    {
    public:
        enum PauseTaskState
        {
            PauseTaskState_Open,
            PauseTaskState_Opening,
            PauseTaskState_Idle,
            PauseTaskState_Action,
            PauseTaskState_Close,
            PauseTaskState_Closing
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
    };
}
