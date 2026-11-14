#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyEFlyer : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyEFlyer, 0x80);
}
