#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::AI
{
    template <typename TState>
    class StateMachine
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<StateMachine<TState>> m_pState;
        MARATHON_INSERT_PADDING(4);
        xpointer<IContext> m_pContext;
        MARATHON_INSERT_PADDING(4);
        be<float> m_Time;

        template <typename TContext = IContext>
        TContext* GetContext()
        {
            return (TContext*)m_pContext.get();
        }
    };
}
