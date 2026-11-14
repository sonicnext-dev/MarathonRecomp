#pragma once

#include <Marathon.inl>
#include <Sonicteam/Enemy/EnemyBase.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Enemy
{
    class EnemyFirstMefiress : public EnemyBase
    {
    public:
        MARATHON_INSERT_PADDING(0x18);
        xpointer<Actor> m_pActorUnk1;
        xpointer<Actor> m_pActorUnk2;
        xpointer<void> m_pZakoressBillboardRenderer; // Sonicteam::Enemy::ZakoressBillboardRenderer
        MARATHON_INSERT_PADDING(0xC);
        SoX::Math::Vector m_UnkVectorZero;
        SoX::Math::Vector m_PostureForce;
        MARATHON_INSERT_PADDING(0x44);
        bool m_UnkFlag;
        MARATHON_INSERT_PADDING(0x8);
        xpointer<void> m_pZakoressCreateServer; // Sonicteam::Enemy::ZakoressCreateServer
        MARATHON_INSERT_PADDING(0xC);
    };

    MARATHON_ASSERT_OFFSETOF(EnemyFirstMefiress, m_pActorUnk1, 0x98);
    MARATHON_ASSERT_OFFSETOF(EnemyFirstMefiress, m_pActorUnk2, 0x9C);
    MARATHON_ASSERT_OFFSETOF(EnemyFirstMefiress, m_pZakoressBillboardRenderer, 0xA0);
    MARATHON_ASSERT_OFFSETOF(EnemyFirstMefiress, m_UnkVectorZero, 0xB0);
    MARATHON_ASSERT_OFFSETOF(EnemyFirstMefiress, m_PostureForce, 0xC0);
    MARATHON_ASSERT_OFFSETOF(EnemyFirstMefiress, m_UnkFlag, 0x114);
    MARATHON_ASSERT_OFFSETOF(EnemyFirstMefiress, m_pZakoressCreateServer, 0x120);
    MARATHON_ASSERT_SIZEOF(EnemyFirstMefiress, 0x130);
}
