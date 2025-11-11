#include "MainMenuTask_patches.h"
#include <ui/achievement_menu.h>
#include <ui/button_window.h>
#include <ui/options_menu.h>
#include <exports.h>

// Sonicteam::MainMenuTask::Update
PPC_FUNC_IMPL(__imp__sub_824FFCF8);
PPC_FUNC(sub_824FFCF8)
{
    auto pMainMenuTask = (Sonicteam::MainMenuTask*)(base + ctx.r3.u32);
    auto pHUDMainMenu = pMainMenuTask->m_pHUDMainMenu;

#ifdef MARATHON_RECOMP_OPTIONS_MENU
    if (pHUDMainMenu)
    {
        // Hide original "OPTIONS" title.
        if (auto pOptionsSelect = pHUDMainMenu->m_pHudTextRoot->Find("options_select", "options"))
            pOptionsSelect->m_Priority = -1.0f;
    }

    if (pMainMenuTask->m_State == Sonicteam::MainMenuTask::MainMenuState_MainMenu && pMainMenuTask->m_MainMenuSelectedIndex == 3)
    {
        if (!OptionsMenu::s_isVisible && (pMainMenuTask->m_PressedButtons.get() & 0x10) != 0)
        {
            OptionsMenu::s_pMainMenuTask = pMainMenuTask;
            OptionsMenu::Open();
    
            Game_PlaySound("main_deside");
    
            pMainMenuTask->m_State = Sonicteam::MainMenuTask::MainMenuState_MainMenu;
            pMainMenuTask->m_PressedButtons = 0;

            guest_stack_var<Sonicteam::Message::HUDMainMenu::MsgSetCursor> msgSetCursor
            (
                Sonicteam::HUDMainMenu::HUDMainMenuState_MainCursorOutro,
                pMainMenuTask->m_MainMenuSelectedIndex
            );

            // Play cursor outro animation.
            pHUDMainMenu->ProcessMessage(msgSetCursor.get());

            guest_stack_var<Sonicteam::Message::HUDMainMenu::MsgTransition> msgTransition
            (
                Sonicteam::HUDMainMenu::HUDMainMenuState_OptionsOutro, 3
            );

            // Play main menu -> options transition.
            pHUDMainMenu->ProcessMessage(msgTransition.get());
        }
    }

    static bool s_isReturningFromOptionsMenu{};

    if (OptionsMenu::s_isVisible)
    {
        pMainMenuTask->m_PressedButtons = 0;

        if (OptionsMenu::s_state == OptionsMenuState::Closing)
            s_isReturningFromOptionsMenu = true;
    }
    else if (s_isReturningFromOptionsMenu)
    {
        if ((pHUDMainMenu->m_CursorFlags.get() & 2) != 0)
        {
            // Prevent inputs leaking from the
            // options menu to the main menu.
            pMainMenuTask->m_PressedButtons = 0;
        }
        else
        {
            s_isReturningFromOptionsMenu = false;
        }
    }
#endif

#ifdef MARATHON_RECOMP_ACHIEVEMENT_MENU
    if (pHUDMainMenu)
    {
        // Hide original "GOLD MEDAL RESULTS" title.
        if (auto pGoldMedalResultSelect = pHUDMainMenu->m_pHudTextRoot->Find("goldmedalresult_select", "goldmedalresult"))
            pGoldMedalResultSelect->m_Priority = -1.0f;
    }

    if (pMainMenuTask->m_State == Sonicteam::MainMenuTask::MainMenuState_GoldMedalResultsOpen)
        AchievementMenu::Open(pMainMenuTask);
#endif

    static float s_buttonWindowTextOffsetY{};
    auto& rButtonWindowTextOffsetY = pMainMenuTask->m_pButtonWindowTask->m_pHUDButtonWindow->m_pHudTextParts->m_OffsetY;

    if (MainMenuTaskPatches::HideButtonWindow)
    {
        static constexpr double HIDE_TEXT_OFFSET = -100000.0f;

        if (rButtonWindowTextOffsetY != HIDE_TEXT_OFFSET)
        {
            // Move original button window text very far off screen to hide it.
            s_buttonWindowTextOffsetY = rButtonWindowTextOffsetY;
            rButtonWindowTextOffsetY = HIDE_TEXT_OFFSET;
        }
    }
    else
    {
        // Restore original button window text offset.
        rButtonWindowTextOffsetY = s_buttonWindowTextOffsetY;
    }

    MainMenuTaskPatches::State = (Sonicteam::MainMenuTask::MainMenuState)pMainMenuTask->m_State.get();

    for (auto& event : MainMenuTaskPatches::Events)
        event->Update(pMainMenuTask, ctx.f1.f64);

    __imp__sub_824FFCF8(ctx, base);
}

bool HUDGoldMedal_ShouldDestroyTable()
{
    return AchievementMenu::s_state == AchievementMenuState::ClosingGoldMedals ||
           AchievementMenu::s_state == AchievementMenuState::ClosingAchievements;
}

bool MainMenuTask_GoldMedalResults_SkipOutro()
{
    return AchievementMenu::s_state == AchievementMenuState::ClosingAchievements;
}
