#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyCTaker : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyCTaker, 0x80);
}
