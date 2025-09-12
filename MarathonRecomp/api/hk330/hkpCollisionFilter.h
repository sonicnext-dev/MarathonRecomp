#pragma once

#include <Marathon.inl>
#include <hk330/hkReferencedObject.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <hk330/hkpRigidBody.h>
#include <hk330/hkArray.h>
#include <hk330/hkpCollidableCollidableFilter.h>
#include <hk330/hkpRayCollidableFilter.h>
#include <hk330/hkpShapeCollectionFilter.h>
#include <hk330/hkpRayShapeCollectionFilter.h>

namespace hk330
{
    class hkpCollisionFilter : public hkReferencedObject, public hkpCollidableCollidableFilter, public hkpShapeCollectionFilter, public hkpRayShapeCollectionFilter, public hkpRayCollidableFilter
    {
    public:
        be<uint32_t> m_FilterType;
        MARATHON_INSERT_PADDING(0xC);
    };
}
