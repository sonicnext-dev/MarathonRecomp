#pragma once

#include <Marathon.inl>
#include <hk330/hkConvexVerticesShape.h>
#include <Sonicteam/SoX/Physics/Havok/ShapeHavok.h>
#include <Sonicteam/SoX/Physics/ConvexShape.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class ConvexShapeHavok : public ConvexShape, public ShapeHavok
    {
    public:
        xpointer<hk330::hkConvexVerticesShape> m_pConvexVerticesShape;
    };

    MARATHON_ASSERT_OFFSETOF(ConvexShapeHavok, m_pConvexVerticesShape, 0x14);
    MARATHON_ASSERT_SIZEOF(ConvexShapeHavok, 0x18);
}
