#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyESearcher : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyESearcher, 0x80);
}
