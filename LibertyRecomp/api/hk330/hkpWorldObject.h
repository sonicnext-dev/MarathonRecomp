#pragma once

#include <Liberty.inl>
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
        LIBERTY_INSERT_PADDING(0x0C);
        hkpLinkedCollidable m_collidable;
        LIBERTY_INSERT_PADDING(8);
        xpointer<const char> m_name;
        hkArray<hkpProperty> m_properties;
    };

    LIBERTY_ASSERT_OFFSETOF(hkpWorldObject, m_world, 0x08);
    LIBERTY_ASSERT_OFFSETOF(hkpWorldObject, m_userData, 0x0C);
    LIBERTY_ASSERT_OFFSETOF(hkpWorldObject, m_collidable, 0x1C);
    LIBERTY_ASSERT_OFFSETOF(hkpWorldObject, m_name, 0x7C);
    LIBERTY_ASSERT_OFFSETOF(hkpWorldObject, m_properties, 0x80);
}
