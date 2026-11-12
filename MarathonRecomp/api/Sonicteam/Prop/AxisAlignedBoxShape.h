#pragma once

#include <Marathon.inl>
#include <Sonicteam/Prop/Shape.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Prop
{
    class AxisAlignedBoxShape : public Shape
    {
    public:
        SoX::Math::Vector m_HalfExtents;
    };

    MARATHON_ASSERT_OFFSETOF(AxisAlignedBoxShape, m_HalfExtents, 0x10);
    MARATHON_ASSERT_SIZEOF(AxisAlignedBoxShape, 0x20);
}
