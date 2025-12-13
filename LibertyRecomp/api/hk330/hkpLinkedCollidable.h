#pragma once

#include <Marathon.inl>
#include <hk330/hkArray.h>
#include <hk330/hkpCollidable.h>

namespace hk330
{
    class hkpLinkedCollidable : public hkpCollidable
    {
    public:
        struct CollisionEntry
        {
            MARATHON_INSERT_PADDING(4);
            xpointer<hkpLinkedCollidable> m_partner;
        };

        hkArray<CollisionEntry> m_collisionEntries;
    };

    MARATHON_ASSERT_OFFSETOF(hkpLinkedCollidable::CollisionEntry, m_partner, 0x04);

    MARATHON_ASSERT_OFFSETOF(hkpLinkedCollidable, m_collisionEntries, 0x4C);
}
