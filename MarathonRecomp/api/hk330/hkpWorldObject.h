#pragma once

#include <Marathon.inl>
#include <hk330/hkArray.h>
#include <hk330/hkpProperty.h>
#include <hk330/hkpWorld.h>
#include <hk330/hkReferencedObject.h>
#include <hk330/hkpLinkedCollidable.h>

namespace hk330
{
    class hkpWorldObject : public hkReferencedObject
    {
    public:
        xpointer<hkpWorld> m_world;
        be<uint32_t> m_userData;
        MARATHON_INSERT_PADDING(0x0C);
        hkpLinkedCollidable m_collidable;
        MARATHON_INSERT_PADDING(8);
        xpointer<const char> m_name;
        hkArray<hkpProperty> m_properties;
    };

    MARATHON_ASSERT_OFFSETOF(hkpWorldObject, m_world, 0x08);
    MARATHON_ASSERT_OFFSETOF(hkpWorldObject, m_userData, 0x0C);
    MARATHON_ASSERT_OFFSETOF(hkpWorldObject, m_collidable, 0x1C);
    MARATHON_ASSERT_OFFSETOF(hkpWorldObject, m_name, 0x7C);
    MARATHON_ASSERT_OFFSETOF(hkpWorldObject, m_properties, 0x80);
}
