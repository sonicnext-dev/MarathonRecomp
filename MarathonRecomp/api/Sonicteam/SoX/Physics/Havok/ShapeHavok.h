#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Physics::Havok
{
    class ShapeHavok
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(ShapeHavok, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(ShapeHavok, 0x04);
}
