#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyEGuardian : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyEGuardian, 0x80);
}
