#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyPhantom.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam::Enemy
{
    class EnemyShotPoint : public EnemyShot
    {
    public:
        MARATHON_INSERT_PADDING(0x20);
        SoX::RefSharedPointer<MyPhantom> m_spPhantom;
        MARATHON_INSERT_PADDING(0xD4);
    };

    MARATHON_ASSERT_OFFSETOF(EnemyShotPoint, m_spPhantom, 0x88);
    MARATHON_ASSERT_SIZEOF(EnemyShotPoint, 0x160);
}
