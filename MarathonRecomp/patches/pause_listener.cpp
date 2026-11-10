#include "pause_listener.h"
#include <ui/game_window.h>
#include <user/config.h>

static bool g_isPaused{};

void PauseListener::SignalPause()
{
    g_isPaused = true;
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
