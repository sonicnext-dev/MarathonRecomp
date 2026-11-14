#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyEGenesisWing : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyEGenesisWing, 0x80);
}
