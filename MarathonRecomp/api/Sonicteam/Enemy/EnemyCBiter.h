#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyCBiter : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyCBiter, 0x80);
}
