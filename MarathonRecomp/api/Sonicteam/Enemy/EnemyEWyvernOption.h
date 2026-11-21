#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyEWyvernOption : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyEWyvernOption, 0x80);
}
