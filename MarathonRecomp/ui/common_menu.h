#pragma once

#include <sdl_listener.h>

class CommonMenu
{
    std::string m_previousTitle{};
    std::string m_previousDesc{};

    ImVec2 m_descPos{};
    ImVec2 m_previousDescPos{};

    bool m_isClosing{};
    double m_time{};
    double m_titleTime{};
    double m_descTime{};

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
            if (event->type == SDL_EVENT_GAMEPAD_AXIS_MOTION && event->gaxis.axis == SDL_GAMEPAD_AXIS_RIGHTX)
                RightStickX = event->gaxis.value / 32767.0f;

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
    std::string Title{};
    std::string Description{};
    bool PlayTransitions{};
    bool ShowDescription{ true };
    bool ShowVersionString{ true };
    bool ReduceDraw{};

    CommonMenu() {}

    CommonMenu(std::string title, std::string desc, bool playTransitions = false)
        : Title(title), Description(desc), PlayTransitions(playTransitions) {}

    void Draw();
    void Open();
    double Close(bool isAnimated = true);
    bool IsOpen() const;
    void SetTitle(std::string title, bool isAnimated = true);
    void SetDescription(std::string desc, bool isAnimated = true);
};
