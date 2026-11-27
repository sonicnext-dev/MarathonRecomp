#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyEGenesisSpotLight : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyEGenesisSpotLight, 0x80);
}
