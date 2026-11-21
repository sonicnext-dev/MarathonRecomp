#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyELiner : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyELiner, 0x80);
}
