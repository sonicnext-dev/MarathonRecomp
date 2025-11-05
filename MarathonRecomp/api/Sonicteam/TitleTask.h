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
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_OFFSETOF(TitleTask, m_State, 0x4C);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_MovieWaitTime, 0x5C);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_Field60, 0x60);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_SelectedIndex, 0x7C);
    MARATHON_ASSERT_OFFSETOF(TitleTask, m_CastIndex, 0xA0);
    MARATHON_ASSERT_SIZEOF(TitleTask, 0xA8);
}
