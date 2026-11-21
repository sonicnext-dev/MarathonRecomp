#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyThirdIblis : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyThirdIblis, 0x80);
}
