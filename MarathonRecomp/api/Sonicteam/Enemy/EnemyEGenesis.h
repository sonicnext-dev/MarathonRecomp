#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyEGenesis : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyEGenesis, 0x80);
}
