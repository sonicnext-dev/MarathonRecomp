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
        MARATHON_INSERT_PADDING(0x48);
        xpointer<State::Machine2> m_pStateMachine;
        MARATHON_INSERT_PADDING(0x1C);
        xpointer<IGauge> m_pGauge;
        MARATHON_INSERT_PADDING(0x208);

        template <typename T>
        inline T* GetGauge();
    };
}

#include <Sonicteam/Player/Object.inl>
