#pragma once

#include <Marathon.inl>
#include <Sonicteam/HUDButtonWindow.h>

namespace Sonicteam
{
    class ButtonWindowTask : public SoX::Engine::Task
    {
    public:
        xpointer<HUDButtonWindow> m_pHUDButtonWindow;
        MARATHON_INSERT_PADDING(8);
    };

    MARATHON_ASSERT_OFFSETOF(ButtonWindowTask, m_pHUDButtonWindow, 0x4C);
    MARATHON_ASSERT_SIZEOF(ButtonWindowTask, 0x58);
}
