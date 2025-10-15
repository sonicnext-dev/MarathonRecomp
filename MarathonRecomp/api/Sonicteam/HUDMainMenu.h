#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class HUDMainMenu : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        enum HUDMainMenuState : uint32_t
        {
            HUDMainMenuState_MainCursorIntro = 98,
            HUDMainMenuState_MainCursorOutro = 99
        };

        MARATHON_INSERT_PADDING(0x1D4);
        be<uint32_t> m_CursorFlags;
        MARATHON_INSERT_PADDING(0x324);
        xpointer<HudTextParts> m_pHudTextRoot;
    };
}
