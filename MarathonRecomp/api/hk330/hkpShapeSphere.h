#pragma once

#include <Marathon.inl>
#include <hk330/hkpShape.h>

namespace hk330
{
    class hkpShapeSphere : public hkpShape
    {
    public:
        be<float> m_radius;
    };

    MARATHON_ASSERT_OFFSETOF(hkpShapeSphere, m_radius, 0xC);
}
