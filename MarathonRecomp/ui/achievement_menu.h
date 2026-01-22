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
    static void Open(Sonicteam::MainMenuTask* pMainMenuTask);
    static void Close();
    static void SetState(AchievementMenuState state);
    static bool IsClosing();
    static void SetGoldMedalResultsVisible(bool isVisible);
};
