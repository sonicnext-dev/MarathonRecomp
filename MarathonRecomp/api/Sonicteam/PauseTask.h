#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class PauseTask : public SoX::Engine::Task
    {
    public:
        enum PauseTaskState
        {
            PauseTaskState_Opening = 0,
            PauseTaskState_Action = 3,
            PauseTaskState_Closing = 5
        };

        MARATHON_INSERT_PADDING(4);
        xpointer<void> m_pHUDPause;
        MARATHON_INSERT_PADDING(0x18);
        be<PauseTaskState> m_State;
        be<uint32_t> m_Flags;
        MARATHON_INSERT_PADDING(0x2C);
        be<uint32_t> m_Buttons;
    };
}
