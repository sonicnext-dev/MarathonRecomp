#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/TechniqueFXL.h>
#include <Sonicteam/MyTexture.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/CsdObject.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class HUDPopupScreen : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        enum HUDPopupScreenState : uint32_t
        {
            HUDPopupScreenState_Opening = 1,
            HUDPopupScreenState_Idle,
            HUDPopupScreenState_Closing
        };

        xpointer<CsdObject> m_pCsdObject;
        SoX::RefSharedPointer<MyTexture> m_pMainTexture;
        SoX::RefSharedPointer<MyTexture> m_pMaskTexture;
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

    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_pCsdObject, 0x54);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_pMainTexture, 0x58);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_pMaskTexture, 0x5C);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_pTechnique, 0x60);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_SceneName, 0x64);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_SpriteName, 0x80);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_State, 0x9C);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_IsClosing, 0xA1);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_ClosingTime, 0xA4);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_X, 0xA8);
    MARATHON_ASSERT_OFFSETOF(HUDPopupScreen, m_Y, 0xAC);
    MARATHON_ASSERT_SIZEOF(HUDPopupScreen, 0xB8);
}
