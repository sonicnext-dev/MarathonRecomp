#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyCGolem : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyCGolem, 0x80);
}
