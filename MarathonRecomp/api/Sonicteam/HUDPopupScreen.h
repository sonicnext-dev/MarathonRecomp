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
}
