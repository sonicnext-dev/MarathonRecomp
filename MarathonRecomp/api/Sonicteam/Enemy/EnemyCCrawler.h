#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>

namespace Sonicteam::Enemy
{
    class EnemyCCrawler : public EnemyBase {};

    MARATHON_ASSERT_SIZEOF(EnemyCCrawler, 0x80);
}
