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
        uint32_t m_userData;
        MARATHON_INSERT_PADDING(0x0C);
        hkpLinkedCollidable m_collidable;
        MARATHON_INSERT_PADDING(8);
        xpointer<const char> m_name;
        hkArray<hkpProperty> m_properties;
    };
}
