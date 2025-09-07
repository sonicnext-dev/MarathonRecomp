#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::AI
{
    template <typename TContext>
    class StateMachine
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<StateMachine<TContext>> m_pState;
        xpointer<TContext> m_pContext;

        template <typename TState>
        TState* GetState()
        {
            return (TState*)m_pState.get();
        }

        TContext* GetContext()
        {
            return (TContext*)m_pContext.get();
        }
    };
}
