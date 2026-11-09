#pragma once

#include <Marathon.inl>
#include <hk330/hkShape.h>

namespace hk330
{
    class hkConvexShape : public hkShape
    {
    public:
        be<float> m_radius;
    };

    MARATHON_ASSERT_OFFSETOF(hkConvexShape, m_radius, 0x0C);
    MARATHON_ASSERT_SIZEOF(hkConvexShape, 0x10);
}
