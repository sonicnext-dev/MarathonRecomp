#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::AI
{
    template <typename TStateContext>
    class StateMachine
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<StateMachine<TStateContext>> m_pState;
        xpointer<TStateContext> m_pContext;

        template <typename TState>
        TState* GetState()
        {
            return (TState*)m_pState.get();
        }

        TStateContext* GetContext()
        {
            return (TStateContext*)m_pContext.get();
        }

        template <typename TContext>
        TContext* GetContext()
        {
            return (TContext*)m_pContext.get();
        }
    };
}
