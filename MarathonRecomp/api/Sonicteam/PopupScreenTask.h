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
        bool m_IsDeleting;
        
        template <typename T = HUDPopupScreen>
        T* GetHUDPopupScreen()
        {
            return static_cast<T*>(m_spHUDPopupScreen.get());
        }
    };

    MARATHON_ASSERT_OFFSETOF(PopupScreenTask, m_spHUDPopupScreen, 0x4C);
    MARATHON_ASSERT_OFFSETOF(PopupScreenTask, m_IsDeleting, 0x50);
    MARATHON_ASSERT_SIZEOF(PopupScreenTask, 0x54);
}
