#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/SoX/AI/State.h>

namespace Sonicteam::SoX::AI
{
    template <typename TContext>
    class StateMachine
    {
    public:
        xpointer<void> m_pVftable;
        boost::shared_ptr<State<TContext>> m_spState;
        MARATHON_INSERT_PADDING(0x14);

        template <typename T = State<TContext>>
        T* GetState()
        {
            return static_cast<T*>(m_spState.get());
        }

        TContext* GetContext()
        {
            return m_spState->GetContext();
        }

        template <typename T = TContext>
        T* GetContext()
        {
            return m_spState->template GetContext<T>();
        }
    };

    MARATHON_ASSERT_OFFSETOF(StateMachine<void>, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(StateMachine<void>, m_spState, 0x04);
    MARATHON_ASSERT_SIZEOF(StateMachine<void>, 0x20);
}
