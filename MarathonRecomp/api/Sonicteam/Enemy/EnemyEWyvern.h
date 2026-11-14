#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyEWyvern : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyEWyvern, 0x80);
}
