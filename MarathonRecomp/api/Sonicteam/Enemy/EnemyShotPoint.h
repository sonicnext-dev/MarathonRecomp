#pragma once

#include <Marathon.inl>

namespace Sonicteam::Enemy
{
    class EnemyShotPoint : public EnemyShot
    {
    public:
        MARATHON_INSERT_PADDING(0x20);
        xpointer<void> m_Field88;
        MARATHON_INSERT_PADDING(0xD4);
    };
}
