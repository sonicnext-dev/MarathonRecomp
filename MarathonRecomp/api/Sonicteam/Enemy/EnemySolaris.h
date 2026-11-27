#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemySolaris : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemySolaris, 0x80);
}
