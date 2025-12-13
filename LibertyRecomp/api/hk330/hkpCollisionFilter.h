#pragma once

#include <Liberty.inl>
#include <hk330/hkArray.h>
#include <hk330/hkpCollidableCollidableFilter.h>
#include <hk330/hkpRayCollidableFilter.h>
#include <hk330/hkpRayShapeCollectionFilter.h>
#include <hk330/hkpRigidBody.h>
#include <hk330/hkpShapeCollectionFilter.h>
#include <hk330/hkReferencedObject.h>

namespace hk330
{
    class hkpCollisionFilter : public hkReferencedObject, public hkpCollidableCollidableFilter, public hkpShapeCollectionFilter, public hkpRayShapeCollectionFilter, public hkpRayCollidableFilter
    {
    public:
        be<uint32_t> m_FilterType;
        LIBERTY_INSERT_PADDING(0x0C);
    };

    LIBERTY_ASSERT_OFFSETOF(hkpCollisionFilter, m_FilterType, 0x18);
}
