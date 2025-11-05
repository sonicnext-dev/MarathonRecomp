#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyShotPoint.h>

namespace Sonicteam::Enemy
{
    class EnemyShotNormal : public EnemyShotPoint {};

    MARATHON_ASSERT_SIZEOF(EnemyShotNormal, 0x160);
}
