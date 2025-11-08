#pragma once

#include <api/Marathon.h>
#include <ui/common_menu.h>

#define MARATHON_RECOMP_ACHIEVEMENT_MENU

enum class AchievementMenuState
{
    GoldMedals,
    Achievements,
    ClosingGoldMedals,
    ClosingAchievements
};

class AchievementMenu
{
public:
    static inline CommonMenu s_commonMenu{};
    static inline AchievementMenuState s_state{};
    static inline Sonicteam::MainMenuTask* s_pMainMenuTask{};
    static inline bool s_isVisible = false;

    static void Draw();
    static void Open();
    static void Close();
    static void SetState(AchievementMenuState state);
    static bool IsClosing();

    static void SetGoldMedalResultsVisible(Sonicteam::MainMenuTask* pMainMenuTask, bool isVisible)
    {
        std::array<int, 4> states{};

        if (isVisible)
        {
            states = { 0, 3, 5, 11 };
        }
        else
        {
            states = { 1, 4, 6, 12 };
        }

        for (auto& state : states)
        {
            guest_stack_var<Sonicteam::Message::HUDGoldMedal::MsgChangeState> msgChangeState(state, pMainMenuTask->m_GoldMedalEpisodeIndex);
            pMainMenuTask->m_pHUDGoldMedal->ProcessMessage(msgChangeState.get());
        }
    }
};
