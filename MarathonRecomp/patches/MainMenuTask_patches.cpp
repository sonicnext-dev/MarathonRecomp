#include "MainMenuTask_patches.h"
#include <ui/options_menu.h>
#include <exports.h>

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

            guest_stack_var<Sonicteam::Message::MsgHUDMainMenuChangeState> msgHUDMainMenuChangeState
            (
                Sonicteam::HUDMainMenu::HUDMainMenuState_MainCursorOutro,
                pMainMenuTask->m_SelectedIndex
            );

            // Play cursor outro animation.
            pMainMenuTask->m_pHUDMainMenu->OnMessageReceived(msgHUDMainMenuChangeState.get());
        }
    }
#endif

    MainMenuTaskPatches::State = (Sonicteam::MainMenuTask::MainMenuState)pMainMenuTask->m_State.get();

    for (auto& event : MainMenuTaskPatches::Events)
        event->Update(pMainMenuTask, ctx.f1.f64);

    if (OptionsMenu::s_isVisible)
    {
        pMainMenuTask->m_PressedButtons = 0;

        if (OptionsMenu::s_state == OptionsMenuState::Closing)
            s_isReturningFromOptionsMenu = true;
    }
    else
    {
        if (s_isReturningFromOptionsMenu)
        {
            guest_stack_var<Sonicteam::Message::MsgHUDMainMenuChangeState> msgHUDMainMenuChangeState
            (
                Sonicteam::HUDMainMenu::HUDMainMenuState_MainCursorIntro,
                pMainMenuTask->m_SelectedIndex
            );

            // Play cursor intro animation.
            pMainMenuTask->m_pHUDMainMenu->OnMessageReceived(msgHUDMainMenuChangeState.get());

            s_isReturningFromOptionsMenu = false;
        }

        __imp__sub_824FFCF8(ctx, base);
    }
}
