#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/ShapeCast.h>
#include <hk330/hkpShape.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class ShapeCastHavok : public Physics::ShapeCast 
    {
        xpointer<hk330::hkpShape> m_HavokShape;
    };

    MARATHON_ASSERT_SIZEOF(ShapeCastHavok, 0x10);
}
