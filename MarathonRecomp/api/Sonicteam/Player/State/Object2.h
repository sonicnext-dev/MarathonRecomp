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

    MARATHON_ASSERT_OFFSETOF(Object2, m_pStateMachine, 0x4);
    MARATHON_ASSERT_OFFSETOF(Object2::Vftable, fpOnStart2, 0x14);
    MARATHON_ASSERT_OFFSETOF(Object2::Vftable, fpOnUpdate2, 0x18);
    MARATHON_ASSERT_OFFSETOF(Object2::Vftable, fpOnEnd2, 0x1C);
    MARATHON_ASSERT_OFFSETOF(Object2::Vftable, fpOnPreUpdate, 0x20);
    MARATHON_ASSERT_OFFSETOF(Object2::Vftable, fpOnPostUpdate, 0x24);
    MARATHON_ASSERT_SIZEOF(Object2, 0x8);
}
