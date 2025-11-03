#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/HUDPopupScreen.h>

namespace Sonicteam
{
    class PopupScreenTask : public SoX::Engine::Task
    {
    public:
        SoX::RefSharedPointer<HUDPopupScreen> m_spHUDPopupScreen;
        
        template <typename T = HUDPopupScreen>
        T* GetHUDPopupScreen()
        {
            return static_cast<T*>(m_spHUDPopupScreen.get());
        }
    };
}
