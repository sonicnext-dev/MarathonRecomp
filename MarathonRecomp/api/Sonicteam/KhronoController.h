#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/KhronoControlInputListener.h>

namespace Sonicteam
{
    class KhronoController : public SoX::Engine::Task
    {
    public:
        xpointer<KhronoControlInputListener> m_pInputListener;
        be<float> m_Time;
        be<float> m_Hour;
        be<float> m_Field58;
        be<float> m_Speed;
        bool m_IsActive;
    };

    MARATHON_ASSERT_OFFSETOF(KhronoController, m_pInputListener, 0x4C);
    MARATHON_ASSERT_OFFSETOF(KhronoController, m_Time, 0x50);
    MARATHON_ASSERT_OFFSETOF(KhronoController, m_Hour, 0x54);
    MARATHON_ASSERT_OFFSETOF(KhronoController, m_Field58, 0x58);
    MARATHON_ASSERT_OFFSETOF(KhronoController, m_Speed, 0x5C);
    MARATHON_ASSERT_OFFSETOF(KhronoController, m_IsActive, 0x60);
    MARATHON_ASSERT_SIZEOF(KhronoController, 0x64);
}
