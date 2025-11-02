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
        
        template <typename HUDPopupType>
        HUDPopupType* GetHUDPopupScreen()
        {
            return static_cast<HUDPopupType*>(m_spHUDPopupScreen.get());
        }
    };

}
