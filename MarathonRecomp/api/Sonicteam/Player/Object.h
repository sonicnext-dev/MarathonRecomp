#pragma once

#include <Marathon.h>
#include <Sonicteam/Player/State/Machine2.h>

namespace Sonicteam::Player
{
    class Object : public Actor
    {
    public:
        stdx::string m_PlayerLua;
        stdx::string m_PlayerPackage;
        MARATHON_INSERT_PADDING(0x8);
        be<uint32_t> m_PlayerIndex;
        MARATHON_INSERT_PADDING(0x48);
        boost::shared_ptr<State::Machine2> m_spStateMachine;
        MARATHON_INSERT_PADDING(0x18);
        boost::shared_ptr<IGauge> m_spGauge;
        MARATHON_INSERT_PADDING(0x204);

        template <typename T>
        inline T* GetGauge();
    };
}

#include <Sonicteam/Player/Object.inl>
