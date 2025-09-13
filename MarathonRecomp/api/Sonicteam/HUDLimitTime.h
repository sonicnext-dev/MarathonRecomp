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
}
