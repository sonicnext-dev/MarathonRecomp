#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/AI/State.h>

namespace Sonicteam::Player::State
{
    class IMachine;

    class Object2 : public SoX::AI::State<IContext>
    {
    public:
        struct Vftable : SoX::AI::State<IContext>::Vftable
        {
            be<uint32_t> fpOnStart2;
            be<uint32_t> fpOnUpdate2;
            be<uint32_t> fpOnEnd2;
            be<uint32_t> fpOnPreUpdate;
            be<uint32_t> fpOnPostUpdate;
        };

        xpointer<IMachine> m_pStateMachine;
    };
}
