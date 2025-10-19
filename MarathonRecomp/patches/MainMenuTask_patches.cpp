#include "MainMenuTask_patches.h"
#include <ui/options_menu.h>
#include <exports.h>

static float g_buttonWindowTextOffsetY{};

// Sonicteam::MainMenuTask::Update
PPC_FUNC_IMPL(__imp__sub_824FFCF8);
PPC_FUNC(sub_824FFCF8)
{
    auto pMainMenuTask = (Sonicteam::MainMenuTask*)(base + ctx.r3.u32);

    static bool s_isReturningFromOptionsMenu{};

#ifdef MARATHON_RECOMP_OPTIONS_MENU
    if (pMainMenuTask->m_State == Sonicteam::MainMenuTask::MainMenuState_MainMenu && pMainMenuTask->m_SelectedIndex == 3)
    {
        if (!OptionsMenu::s_isVisible && (pMainMenuTask->m_PressedButtons.get() & 0x10) != 0)
        {
            OptionsMenu::Open();
    
            Game_PlaySound("main_deside");
    
            pMainMenuTask->m_State = Sonicteam::MainMenuTask::MainMenuState_MainMenu;
            pMainMenuTask->m_PressedButtons = 0;

            guest_stack_var<Sonicteam::Message::MsgHUDMainMenuSetCursor> msgHUDMainMenuSetCursor
            (
                Sonicteam::HUDMainMenu::HUDMainMenuState_MainCursorOutro,
                pMainMenuTask->m_SelectedIndex
            );

            // Play cursor outro animation.
            pMainMenuTask->m_pHUDMainMenu->ProcessMessage(msgHUDMainMenuSetCursor.get());

            guest_stack_var<Sonicteam::Message::MsgHUDMainMenuTransition> msgHUDMainMenuTransition
            (
                Sonicteam::HUDMainMenu::HUDMainMenuState_OptionsOutro, 3
            );

            // Play main menu -> options transition.
            pMainMenuTask->m_pHUDMainMenu->ProcessMessage(msgHUDMainMenuTransition.get());
        }
    }
#endif

    auto& rButtonWindowTextOffsetY = pMainMenuTask->m_pButtonWindowTask->m_pHUDButtonWindow->m_pHudTextParts->m_OffsetY;

    if (OptionsMenu::s_isVisible)
    {
        pMainMenuTask->m_PressedButtons = 0;

        switch (OptionsMenu::s_state)
        {
            // Move original button window text very far off screen to "hide" it.
            case OptionsMenuState::Opening:
                g_buttonWindowTextOffsetY = rButtonWindowTextOffsetY;
                rButtonWindowTextOffsetY = -100000.0f;
                break;

            case OptionsMenuState::Closing:
                s_isReturningFromOptionsMenu = true;
                break;
        }
    }
    else
    {
        if (s_isReturningFromOptionsMenu)
        {
            // Restore original button window text offset.
            rButtonWindowTextOffsetY = g_buttonWindowTextOffsetY;

            guest_stack_var<Sonicteam::Message::MsgHUDMainMenuSetCursor> msgHUDMainMenuSetCursor
            (
                Sonicteam::HUDMainMenu::HUDMainMenuState_MainCursorIntro,
                pMainMenuTask->m_SelectedIndex
            );

            // Play cursor intro animation.
            pMainMenuTask->m_pHUDMainMenu->ProcessMessage(msgHUDMainMenuSetCursor.get());

            guest_stack_var<Sonicteam::Message::MsgHUDMainMenuTransition> msgHUDMainMenuTransition
            (
                Sonicteam::HUDMainMenu::HUDMainMenuState_OptionsIntro, 3
            );

            // Play options -> main menu transition.
            pMainMenuTask->m_pHUDMainMenu->ProcessMessage(msgHUDMainMenuTransition.get());

            s_isReturningFromOptionsMenu = false;
        }
    }

    MainMenuTaskPatches::State = (Sonicteam::MainMenuTask::MainMenuState)pMainMenuTask->m_State.get();

    for (auto& event : MainMenuTaskPatches::Events)
        event->Update(pMainMenuTask, ctx.f1.f64);

    __imp__sub_824FFCF8(ctx, base);
}
