#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyTexture.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class HUDPopupScreen : public SoX::Engine::Task
    {
    public:
        xpointer<CsdObject> m_pCsdObject;
        xpointer<MyTexture> m_pMainTexture;
        xpointer<MyTexture> m_pMaskTexture;
        xpointer<void> m_pTechnique;
        MARATHON_INSERT_PADDING(8);
        stdx::string m_SceneName;
        stdx::string m_SpriteName;
        MARATHON_INSERT_PADDING(4);
        be<float> m_X;
        be<float> m_Y;
        MARATHON_INSERT_PADDING(8);
    };
}
