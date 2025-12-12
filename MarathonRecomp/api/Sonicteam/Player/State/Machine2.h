#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/IContext.h>
#include <Sonicteam/Player/State/IMachine.h>
#include <Sonicteam/SoX/AI/StateMachine.h>
#include <api/stdx/vector.h>
#include <api/stdx/deque.h>

namespace Sonicteam::Player::State
{
    //std::function
    using ConstructFunction = xpointer<boost::shared_ptr<SoX::AI::StateMachine<IContext>>()>;

    class Machine2 : public SoX::AI::StateMachine<IContext>, public IMachine
    {
    public:
        stdx::vector<ConstructFunction> m_vInitialStates;
        be<uint32_t> m_PreState;
        be<uint32_t> m_PostState;
        bool m_IsNewStateQueue; // place current state to deqeue, then use Construct New State
        bool m_DoPendingState; // does not remove element m_dPendingStates
        bool m_DoPendingStatePop; //does remove element m_dPendingStates also m_dspPendingStates
        bool m_Flag4;
        stdx::deque<uint32_t> m_dPendingStates;
        boost::shared_ptr<IContext> m_spContext;
      
    };

    MARATHON_ASSERT_OFFSETOF(Machine2, m_vInitialStates, 0x40);
    MARATHON_ASSERT_OFFSETOF(Machine2, m_PreState, 0x50);
    MARATHON_ASSERT_OFFSETOF(Machine2, m_PostState, 0x54);
    MARATHON_ASSERT_OFFSETOF(Machine2, m_IsNewStateQueue, 0x58);
    MARATHON_ASSERT_OFFSETOF(Machine2, m_DoPendingState, 0x59);
    MARATHON_ASSERT_OFFSETOF(Machine2, m_DoPendingStatePop, 0x5A);
    MARATHON_ASSERT_OFFSETOF(Machine2, m_Flag4, 0x5B);
    MARATHON_ASSERT_OFFSETOF(Machine2, m_dPendingStates, 0x5C);
    MARATHON_ASSERT_OFFSETOF(Machine2, m_spContext, 0x70);
    MARATHON_ASSERT_SIZEOF(Machine2, 0x78);
}
