#include "pause_listener.h"
#include <ui/game_window.h>
#include <user/config.h>
#include <app.h>

static bool g_isPaused{};

void PauseListener::SignalPause()
{
    // Only signal pause in game mode.
    g_isPaused = App::s_pApp && App::s_pApp->GetGame();
}

void SetPause(PPCRegister& r25)
{
    if (!Config::AllowBackgroundInput && !GameWindow::s_isFocused)
        r25.u32 = 1;

    if (g_isPaused)
    {
        r25.u32 = 1;
        g_isPaused = false;
    }
}
