#pragma once

#include <Marathon.h>
#include <Sonicteam/Player/State/Machine2.h>

namespace Sonicteam::Player
{
    class Object : public Actor
    {
    public:
        MARATHON_INSERT_PADDING(0x40);
        be<uint32_t> m_PlayerIndex;
        MARATHON_INSERT_PADDING(0x2C);
        bool m_IsPlayer;
        bool m_IsPosture;
        bool m_IsAmigo;
        MARATHON_INSERT_PADDING(0x19);
        boost::shared_ptr<State::Machine2> m_spStateMachine;
        MARATHON_INSERT_PADDING(0x10);
        be<uint32_t> m_SetupModuleIndexPrefix;
        be<uint32_t> m_SetupModuleIndexPostfix;
        boost::shared_ptr<IGauge> m_spGauge;
        MARATHON_INSERT_PADDING(0x204);

        template <typename T>
        inline T* GetGauge();
    };
}

#include <Sonicteam/Player/Object.inl>
