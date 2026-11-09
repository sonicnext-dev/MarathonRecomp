#include <api/Marathon.h>
#include <os/logger.h>
#include <user/config.h>
#include <app.h>
#include <sdl_listener.h>

static class UIToggleListener : public SDLEventListener
{
public:
    bool OnSDLEvent(SDL_Event* event) override
    {
        if (!App::s_pApp || !Config::ToggleUIOnF8)
            return false;

        switch (event->type)
        {
            case SDL_EVENT_KEY_DOWN:
            {
                if (event->key.key != SDLK_F8 || event->key.repeat)
                    break;

                App::s_isDrawUI = !App::s_isDrawUI;

                if (!App::s_isDrawUI)
                {
                    if (const auto pGame = App::s_pApp->GetGame())
                    {
                        if (const auto pPopupScreenTask = pGame->m_rPopupScreenTask.m_pRef)
                        {
                            pPopupScreenTask->m_spHUDPopupScreen->m_State = Sonicteam::HUDPopupScreen::HUDPopupScreenState_Deleting;
                            pPopupScreenTask->m_IsDeleting = true;
                        }
                    }
                }

                LOGFN("UI: {}", App::s_isDrawUI ? "Enabled" : "Disabled");

                break;
            }
        }

        return false;
    }
}
g_uiToggleListener{};
