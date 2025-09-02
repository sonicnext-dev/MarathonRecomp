#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class TitleTask : public SoX::Engine::Task
    {
    public:
        static constexpr float ms_DefaultMovieWaitTime = 30.0f;

        enum TitleState : uint32_t
        {
            TitleState_Open,
            TitleState_Wait,
            TitleState_PressStart,
            TitleState_OptionsOpen = 5,
            TitleState_OptionsWait,
            TitleState_OptionsProceed = 7,
            TitleState_Proceed,
            TitleState_Outro = 13
        };

        be<TitleState> m_State;
        MARATHON_INSERT_PADDING(0x0C);
        be<float> m_MovieWaitTime;
        be<uint32_t> m_Field60;
        MARATHON_INSERT_PADDING(0x18);
        be<uint32_t> m_SelectedIndex;
        MARATHON_INSERT_PADDING(0x20);
        be<uint32_t> m_CastIndex;
    };
}
