#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Physics/Entity.h>
#include <Sonicteam/SoX/Physics/EntityRef.h>

namespace Sonicteam::Player
{
    class ICollisionListener
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpSetPostureRequestFlag;
            be<uint32_t> fpUpdate;
            be<uint32_t> fpGetContactPosition;
            be<uint32_t> fpGetContactNormal;
            be<uint32_t> fpGetLength;
            be<uint32_t> fpGetCollisionFlag;
            be<uint32_t> GetContactEntity;
            be<uint32_t> fpGetCurrentSurfaceContacts;
            be<uint32_t> fpGetRayFlag;
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0xC);
        be<uint32_t> m_Flag1;
        be<uint32_t> m_CurrentSurfaceContacts;
        be<uint32_t> m_PostureRequestFlag;
        SoX::Math::Vector m_CollisionNormal;

        void SetPostureRequestFlag(uint32_t postureRequestFlag);

        void Update();

        SoX::Math::Vector GetContactPosition();

        SoX::Math::Vector GetContactNormal();

        float GetLength(float normalize);    // GetLength(), of what?

        uint32_t GetCollisionFlag();    // GetCollisionFlag()

        SoX::Physics::EntityRefWrapper GetContactEntity();

        uint32_t GetCurrentSurfaceContacts();

        uint32_t GetRayFlag();
    };

    MARATHON_ASSERT_SIZEOF(ICollisionListener, 0x30);
    MARATHON_ASSERT_OFFSETOF(ICollisionListener, m_Flag1, 0x10);
    MARATHON_ASSERT_OFFSETOF(ICollisionListener, m_CurrentSurfaceContacts, 0x14);
    MARATHON_ASSERT_OFFSETOF(ICollisionListener, m_PostureRequestFlag, 0x18);
    MARATHON_ASSERT_OFFSETOF(ICollisionListener, m_CollisionNormal, 0x20);
}

#include "ICollisionListener.inl"
