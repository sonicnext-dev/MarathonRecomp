#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/AI/State.h>
#include <stdx/deque.h>

namespace Sonicteam::SoX::AI
{
    template <typename TStateContext>
    class StateMachine
    {
    public:
        xpointer<void> m_pVftable;
        boost::shared_ptr<Sonicteam::SoX::AI::State<TStateContext>> m_spState;
        stdx::deque<boost::shared_ptr<Sonicteam::SoX::AI::State<TStateContext>>> m_dspPendingStates;

        template <typename TState>
        TState* GetState()
        {
            return (TState*)m_spState.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(StateMachine<void>, m_pVftable, 0x0);
    MARATHON_ASSERT_OFFSETOF(StateMachine<void>, m_spState, 0x4);
    MARATHON_ASSERT_OFFSETOF(StateMachine<void>, m_dspPendingStates, 0xC);
    MARATHON_ASSERT_SIZEOF(StateMachine<void>, 0x20);
}
