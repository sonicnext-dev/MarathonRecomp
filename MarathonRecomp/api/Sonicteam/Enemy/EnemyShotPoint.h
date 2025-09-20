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
}
