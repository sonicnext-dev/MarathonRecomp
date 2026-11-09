#pragma once

#include <Marathon.inl>
#include <hk330/hkArray.h>
#include <hk330/hkCollidable.h>

namespace hk330
{
    class hkLinkedCollidable : public hkCollidable
    {
    public:
        struct CollisionEntry
        {
            MARATHON_INSERT_PADDING(0x04);
            xpointer<hkLinkedCollidable> m_partner;
        };

        hkArray<CollisionEntry> m_collisionEntries;
    };

    MARATHON_ASSERT_OFFSETOF(hkLinkedCollidable::CollisionEntry, m_partner, 0x04);
    MARATHON_ASSERT_SIZEOF(hkLinkedCollidable::CollisionEntry, 0x08);

    MARATHON_ASSERT_OFFSETOF(hkLinkedCollidable, m_collisionEntries, 0x4C);
    MARATHON_ASSERT_SIZEOF(hkLinkedCollidable, 0x58);
}
