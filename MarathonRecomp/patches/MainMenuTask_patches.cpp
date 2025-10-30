#include "MainMenuTask_patches.h"
#include <ui/options_menu.h>
#include <exports.h>

// Sonicteam::MainMenuTask::Update
PPC_FUNC_IMPL(__imp__sub_824FFCF8);
PPC_FUNC(sub_824FFCF8)
{
    auto pMainMenuTask = (Sonicteam::MainMenuTask*)(base + ctx.r3.u32);
    auto pHUDMainMenu = pMainMenuTask->m_pHUDMainMenu;

#ifdef MARATHON_RECOMP_OPTIONS_MENU
    if (pMainMenuTask->m_State == Sonicteam::MainMenuTask::MainMenuState_MainMenu && pMainMenuTask->m_MainMenuSelectedIndex == 3)
    {
        if (!OptionsMenu::s_isVisible && (pMainMenuTask->m_PressedButtons.get() & 0x10) != 0)
        {
            OptionsMenu::s_pMainMenuTask = pMainMenuTask;
            OptionsMenu::Open();
    
            Game_PlaySound("main_deside");
    
            pMainMenuTask->m_State = Sonicteam::MainMenuTask::MainMenuState_MainMenu;
            pMainMenuTask->m_PressedButtons = 0;

            guest_stack_var<Sonicteam::Message::MsgHUDMainMenuSetCursor> msgHUDMainMenuSetCursor
            (
                Sonicteam::HUDMainMenu::HUDMainMenuState_MainCursorOutro,
                pMainMenuTask->m_MainMenuSelectedIndex
            );

            // Play cursor outro animation.
            pHUDMainMenu->ProcessMessage(msgHUDMainMenuSetCursor.get());

            guest_stack_var<Sonicteam::Message::MsgHUDMainMenuTransition> msgHUDMainMenuTransition
            (
                Sonicteam::HUDMainMenu::HUDMainMenuState_OptionsOutro, 3
            );

            // Play main menu -> options transition.
            pHUDMainMenu->ProcessMessage(msgHUDMainMenuTransition.get());
        }
    }

    static bool s_isReturningFromOptionsMenu{};
    static float s_buttonWindowTextOffsetY{};

    auto& rButtonWindowTextOffsetY = pMainMenuTask->m_pButtonWindowTask->m_pHUDButtonWindow->m_pHudTextParts->m_OffsetY;

    if (pHUDMainMenu)
    {
        // Hide original "OPTIONS" title.
        if (auto pOptionsSelect = pHUDMainMenu->m_pHudTextRoot->Find("options_select", "options"))
            pOptionsSelect->m_Priority = -1.0f;
    }

    if (OptionsMenu::s_isVisible)
    {
        pMainMenuTask->m_PressedButtons = 0;

        switch (OptionsMenu::s_state)
        {
            case OptionsMenuState::Opening:
            {
                static constexpr double HIDE_TEXT_OFFSET = -100000.0f;

                if (rButtonWindowTextOffsetY == HIDE_TEXT_OFFSET)
                    break;

                // Move original button window text very far off screen to hide it.
                s_buttonWindowTextOffsetY = rButtonWindowTextOffsetY;
                rButtonWindowTextOffsetY = HIDE_TEXT_OFFSET;

                break;
            }

            case OptionsMenuState::Closing:
                s_isReturningFromOptionsMenu = true;
                break;
        }
    }
    else if (s_isReturningFromOptionsMenu)
    {
        // Restore original button window text offset.
        rButtonWindowTextOffsetY = s_buttonWindowTextOffsetY;

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

    MainMenuTaskPatches::State = (Sonicteam::MainMenuTask::MainMenuState)pMainMenuTask->m_State.get();

    for (auto& event : MainMenuTaskPatches::Events)
        event->Update(pMainMenuTask, ctx.f1.f64);

    __imp__sub_824FFCF8(ctx, base);
}
