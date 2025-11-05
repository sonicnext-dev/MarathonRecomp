#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/TechniqueFXL.h>
#include <Sonicteam/MyTexture.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class HUDPopupScreen : public SoX::Engine::Task
    {
    public:
        enum HUDPopupScreenState : uint32_t
        {
            HUDPopupScreenState_Opening = 1,
            HUDPopupScreenState_Idle,
            HUDPopupScreenState_Closing
        };

        xpointer<CsdObject> m_pCsdObject;
        xpointer<MyTexture> m_pMainTexture;
        xpointer<MyTexture> m_pMaskTexture;
        xpointer<SoX::Graphics::TechniqueFXL> m_pTechnique;
        stdx::string m_SceneName;
        stdx::string m_SpriteName;
        be<HUDPopupScreenState> m_State;
        MARATHON_INSERT_PADDING(1);
        bool m_IsClosing;
        MARATHON_INSERT_PADDING(2);
        be<float> m_ClosingTime;
        be<float> m_X;
        be<float> m_Y;
        MARATHON_INSERT_PADDING(8);
    };

    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_pCsdObject, 0x4C);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_pMainTexture, 0x50);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_pMaskTexture, 0x54);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_pTechnique, 0x58);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_SceneName, 0x5C);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_SpriteName, 0x78);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_State, 0x94);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_IsClosing, 0x99);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_ClosingTime, 0x9C);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_X, 0xA0);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_Y, 0xA4);
    MARATHON_ASSERT_SIZEOF(HUDPopupScreen, 0xB0);
}
