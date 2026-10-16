#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class HUDPause : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        xpointer<CsdObject> m_pCsdObject;
        MARATHON_INSERT_PADDING(0xA0);
        be<float> m_TextPriority;
        MARATHON_INSERT_PADDING(0x40);
        xpointer<HudTextParts> m_pHudTextRoot;
        bool m_ShowMissionWindow;
        MARATHON_INSERT_PADDING(0x07);
    };

    MARATHON_ASSERT_OFFSETOF(HUDPause, m_pCsdObject, 0x54);
    MARATHON_ASSERT_OFFSETOF(HUDPause, m_TextPriority, 0xF8);
    MARATHON_ASSERT_OFFSETOF(HUDPause, m_pHudTextRoot, 0x13C);
    MARATHON_ASSERT_OFFSETOF(HUDPause, m_ShowMissionWindow, 0x140);
    MARATHON_ASSERT_SIZEOF(HUDPause, 0x148);
}
