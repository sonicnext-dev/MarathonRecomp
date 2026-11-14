#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyECanon : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyECanon, 0x80);
}
