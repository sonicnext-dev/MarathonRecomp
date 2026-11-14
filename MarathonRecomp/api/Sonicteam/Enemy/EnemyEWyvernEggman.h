#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyEWyvernEggman : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyEWyvernEggman, 0x80);
}
