#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyEBomber : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyEBomber, 0x80);
}
