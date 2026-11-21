#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyERounder : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyERounder, 0x80);
}
