#include "MainMenuTask_patches.h"
#include <hid/message/messages.h>
#include <hid/hid.h>
#include <ui/achievement_menu.h>
#include <ui/button_window.h>
#include <ui/options_menu.h>
#include <exports.h>

static bool g_isMultiplayerP1KeyboardUser{};

// Sonicteam::MainMenuTask::Update
PPC_FUNC_IMPL(__imp__sub_824FFCF8);
PPC_FUNC(sub_824FFCF8)
{
    const auto pMainMenuTask = (Sonicteam::MainMenuTask*)(base + ctx.r3.u32);
    const auto pHUDMainMenu = pMainMenuTask->m_pHUDMainMenu;
    const auto pDoc = pMainMenuTask->GetDoc<Sonicteam::DocMarathonState>();

#ifdef MARATHON_RECOMP_OPTIONS_MENU
    if (pHUDMainMenu)
    {
        // Hide original "OPTIONS" title.
        if (auto pOptionsSelect = pHUDMainMenu->m_pHudTextRoot->Find("options_select", "options"))
            pOptionsSelect->m_Priority = -1.0f;
    }

    switch (pMainMenuTask->m_State)
    {
        case Sonicteam::MainMenuTask::MainMenuState_MainMenu:
        {
            if (pMainMenuTask->m_MainMenuSelectedIndex != 3)
                break;

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

            break;
        }

        case Sonicteam::MainMenuTask::MainMenuState_Multiplayer:
        {
            const auto& rPadState = pDoc->m_vspInputManagers[0]->PadState;

            if (rPadState.IsPressed(Sonicteam::SoX::Input::PadButton_South) ||
                rPadState.IsPressed(Sonicteam::SoX::Input::PadButton_West))
            {
                const auto pMutator = HID::GetManager()->GetMutator();

                // Assign keyboard to Player 1, if they entered the character select via the keyboard.
                if (pMutator && pMutator->GetType() != HID::PeripheralType::Gamepad)
                {
                    HID::GetManager()->ProcessMessage<HID::MsgSetKeyboardUser>(0);
                    g_isMultiplayerP1KeyboardUser = true;
                }
            }

            break;
        }

        case Sonicteam::MainMenuTask::MainMenuState_Tag:
        case Sonicteam::MainMenuTask::MainMenuState_Battle:
        {
            if (pDoc->m_vspInputManagers[0]->PadState.IsPressed(Sonicteam::SoX::Input::PadButton_East))
            {
                if (g_isMultiplayerP1KeyboardUser)
                {
                    // Restore original gamepad locations from keyboard assignment.
                    HID::GetManager()->ProcessMessage<HID::MsgPopGamepads>(0);
                    g_isMultiplayerP1KeyboardUser = false;
                }
            }

            break;
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

    if (MainMenuTaskPatches::s_hideButtonWindow)
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

    MainMenuTaskPatches::s_state = (Sonicteam::MainMenuTask::MainMenuState)pMainMenuTask->m_State.get();

    for (auto& event : MainMenuTaskPatches::s_events)
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
