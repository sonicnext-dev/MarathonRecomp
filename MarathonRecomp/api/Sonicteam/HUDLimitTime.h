#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class HUDLimitTime : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        static constexpr float ms_AlertThreshold = 10.0f;

        xpointer<CsdObject> m_pCsdObject;
        be<float> m_X;
        be<float> m_Y;
        MARATHON_INSERT_PADDING(0x30);
        be<float> m_Time;
        bool m_IsAboveAlertThreshold;
    };

    MARATHON_ASSERT_OFFSETOF(HUDLimitTime, m_pCsdObject, 0x54);
    MARATHON_ASSERT_OFFSETOF(HUDLimitTime, m_X, 0x58);
    MARATHON_ASSERT_OFFSETOF(HUDLimitTime, m_Y, 0x5C);
    MARATHON_ASSERT_OFFSETOF(HUDLimitTime, m_Time, 0x90);
    MARATHON_ASSERT_OFFSETOF(HUDLimitTime, m_IsAboveAlertThreshold, 0x94);
    MARATHON_ASSERT_SIZEOF(HUDLimitTime, 0x98);
}
