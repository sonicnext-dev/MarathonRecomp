#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::AI
{
    template <typename T>
    class StateMachine
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<StateMachine<T>> m_pState;
        MARATHON_INSERT_PADDING(0x10);
        be<float> m_Time;
        MARATHON_INSERT_PADDING(4);
    };
}
