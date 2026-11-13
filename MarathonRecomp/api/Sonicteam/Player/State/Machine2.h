#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/IContext.h>
#include <Sonicteam/Player/State/IMachine.h>
#include <Sonicteam/SoX/AI/StateMachine.h>

namespace Sonicteam::Player::State
{
    class Machine2 : public SoX::AI::StateMachine<IContext>, public IMachine
    {
    public:
        MARATHON_INSERT_PADDING(0x10);
        be<uint32_t> m_StateID;
        MARATHON_INSERT_PADDING(0x24);
    };

    MARATHON_ASSERT_OFFSETOF(Machine2, m_StateID, 0x50);
    MARATHON_ASSERT_SIZEOF(Machine2, 0x78);
}
