#pragma once

#include <Marathon.inl>

namespace Sonicteam::Enemy
{
    class EnemyShot : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x1C);
    };

    MARATHON_ASSERT_SIZEOF(EnemyShot, 0x68);
}
