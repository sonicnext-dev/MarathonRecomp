#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyECerberus : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyECerberus, 0x80);
}
