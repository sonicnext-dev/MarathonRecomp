#pragma once

#include <Marathon.inl>
#include <hk330/hkArray.h>
#include <hk330/hkCollidableCollidableFilter.h>
#include <hk330/hkRayCollidableFilter.h>
#include <hk330/hkRayShapeCollectionFilter.h>
#include <hk330/hkReferencedObject.h>
#include <hk330/hkRigidBody.h>
#include <hk330/hkShapeCollectionFilter.h>

namespace hk330
{
    class hkCollisionFilter : public hkReferencedObject, public hkCollidableCollidableFilter, public hkShapeCollectionFilter, public hkRayShapeCollectionFilter, public hkRayCollidableFilter
    {
    public:
        be<uint32_t> FilterType;
        MARATHON_INSERT_PADDING(0x0C);
    };

    MARATHON_ASSERT_OFFSETOF(hkCollisionFilter, FilterType, 0x18);
}
