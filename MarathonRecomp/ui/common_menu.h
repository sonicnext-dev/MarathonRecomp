#pragma once

#include <sdl_listener.h>

class CommonMenu
{
    const char* m_previousTitle{};
    const char* m_previousDesc{};

    ImVec2 m_descPos{};
    ImVec2 m_previousDescPos{};

    double m_appearTime{};
    double m_titleAppearTime{};
    double m_descAppearTime{};

    bool m_isDescScrolling{};
    bool m_isDescManualScrolling{};
    float m_descScrollOffset{};
    float m_descScrollTimer{};
    float m_descScrollDirection{ 1.0f };

    class CommonMenuInputListener : public SDLEventListener
    {
    public:
        float RightStickX{};

        bool OnSDLEvent(SDL_Event* event) override
        {
            if (event->type == SDL_CONTROLLERAXISMOTION && event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
                RightStickX = event->caxis.value / 32767.0f;

            return false;
        }
    }
    m_inputListener{};

    void resetDescScroll()
    {
        m_isDescScrolling = false;
        m_descScrollOffset = 0.0f;
        m_descScrollTimer = 0.0f;
        m_descScrollDirection = 1.0f;
    }

public:
    const char* Title{};
    const char* Description{};
    bool PlayIntroAnim{};
    bool ShowVersionString{ true };

    CommonMenu() {}

    CommonMenu(const char* title, const char* desc = nullptr, bool playIntroAnim = false, bool showVersionString = true)
        : Title(title), Description(desc), PlayIntroAnim(playIntroAnim), ShowVersionString(showVersionString) {}

    void Open();
    void Draw();
    void SetTitle(const char* title, bool isAnimated = true);
    void SetDescription(const char* desc, bool isAnimated = true);
};
