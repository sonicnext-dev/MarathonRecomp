#pragma once

#include <Marathon.inl>
#include <hk330/hkArray.h>
#include <hk330/hkLinkedCollidable.h>
#include <hk330/hkProperty.h>
#include <hk330/hkReferencedObject.h>
#include <hk330/hkWorld.h>

namespace hk330
{
    class hkWorldObject : public hkReferencedObject
    {
    public:
        xpointer<hkWorld> m_world;
        be<uint32_t> m_userData;
        MARATHON_INSERT_PADDING(0x0C);
        hkLinkedCollidable m_collidable;
        MARATHON_INSERT_PADDING(0x08);
        xpointer<const char> m_name;
        hkArray<hkProperty> m_properties;
    };

    MARATHON_ASSERT_OFFSETOF(hkWorldObject, m_world, 0x08);
    MARATHON_ASSERT_OFFSETOF(hkWorldObject, m_userData, 0x0C);
    MARATHON_ASSERT_OFFSETOF(hkWorldObject, m_collidable, 0x1C);
    MARATHON_ASSERT_OFFSETOF(hkWorldObject, m_name, 0x7C);
    MARATHON_ASSERT_OFFSETOF(hkWorldObject, m_properties, 0x80);
    MARATHON_ASSERT_SIZEOF(hkWorldObject, 0x8C);
}
