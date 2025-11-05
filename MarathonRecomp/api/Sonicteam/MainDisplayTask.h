#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class MainDisplayTask : public SoX::Engine::Task
    {
    public:
        xpointer<HUDMainDisplay> m_pHUDMainDisplay;
        MARATHON_INSERT_PADDING(0x20);
    };

    MARATHON_ASSERT_OFFSETOF(MainDisplayTask, m_pHUDMainDisplay, 0x4C);
    MARATHON_ASSERT_SIZEOF(MainDisplayTask, 0x70);
}
