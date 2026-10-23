#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::AI
{
    template <typename TContext>
    class StateMachine;

    template <typename TContext>
    class State
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<StateMachine<TContext>> m_pStateMachine;
        xpointer<TContext> m_pContext;

        TContext* GetContext()
        {
            return static_cast<TContext*>(m_pContext.get());
        }

        template <typename T = TContext>
        T* GetContext()
        {
            return static_cast<T*>(m_pContext.get());
        }
    };

    MARATHON_ASSERT_OFFSETOF(State<void>, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(State<void>, m_pStateMachine, 0x04);
    MARATHON_ASSERT_OFFSETOF(State<void>, m_pContext, 0x08);
    MARATHON_ASSERT_SIZEOF(State<void>, 0x0C);
}
