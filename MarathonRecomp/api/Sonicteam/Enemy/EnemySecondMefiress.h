#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyFirstMefiress.h>

namespace Sonicteam::Enemy
{
    class EnemySecondMefiress : public EnemyFirstMefiress
    {
    public:
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_SIZEOF(EnemySecondMefiress, 0x140);
}
