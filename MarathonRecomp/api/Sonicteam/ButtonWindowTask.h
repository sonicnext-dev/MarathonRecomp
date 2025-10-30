#pragma once

#include <Marathon.inl>
#include <Sonicteam/HUDButtonWindow.h>

namespace Sonicteam
{
    class ButtonWindowTask : public SoX::Engine::Task
    {
    public:
        xpointer<HUDButtonWindow> m_pHUDButtonWindow;
    };
}
