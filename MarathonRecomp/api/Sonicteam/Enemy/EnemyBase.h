#pragma once

#include "Sonicteam/SoX/AI/StateMachine.h"

#include <Marathon.inl>
#include <Sonicteam/Actor.h>

namespace Sonicteam::Enemy
{
    class EnemyBase : public Actor
    {
    public:
        MARATHON_INSERT_PADDING(0x4);
        xpointer<SoX::AI::StateMachine<void>> m_pStateMachine; // Enemy::EnemyContext
        MARATHON_INSERT_PADDING(0x20);
    };

    MARATHON_ASSERT_OFFSETOF(EnemyBase, m_pStateMachine, 0x5C);
    MARATHON_ASSERT_SIZEOF(EnemyBase, 0x80);
}
