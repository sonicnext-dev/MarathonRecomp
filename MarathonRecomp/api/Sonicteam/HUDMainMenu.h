#pragma once

#include <Marathon.inl>
#include <Sonicteam/CsdObject.h>
#include <Sonicteam/HudTextParts.h>

namespace Sonicteam
{
    class HUDMainMenu : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        enum HUDMainMenuState : uint32_t
        {
            HUDMainMenuState_OptionsOutro = 5,
            HUDMainMenuState_SinglePlayerTextOutro = 8,
            HUDMainMenuState_SinglePlayerCursorIntro = 86,
            HUDMainMenuState_OptionsIntro = 90,
            HUDMainMenuState_MainCursorIntro = 98,
            HUDMainMenuState_MainCursorOutro = 99,
            HUDMainMenuState_GoldMedalResultsCursorOutro = 101
        };

        MARATHON_INSERT_PADDING(0x20);
        xpointer<CsdObject> m_pCsdObject;
        MARATHON_INSERT_PADDING(0x1B0);
        be<uint32_t> m_CursorFlags;
        MARATHON_INSERT_PADDING(0x32C);
        xpointer<HudTextParts> m_pHudTextRoot;
        MARATHON_INSERT_PADDING(0x48C);
    };

    MARATHON_ASSERT_OFFSETOF(HUDMainMenu, m_pCsdObject, 0x74);
    MARATHON_ASSERT_OFFSETOF(HUDMainMenu, m_CursorFlags, 0x228);
    MARATHON_ASSERT_OFFSETOF(HUDMainMenu, m_pHudTextRoot, 0x558);
    MARATHON_ASSERT_SIZEOF(HUDMainMenu, 0x9E8);
}
