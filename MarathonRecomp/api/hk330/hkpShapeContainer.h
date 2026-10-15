#pragma once

#include <Marathon.inl>

namespace hk330
{
    class hkpShapeContainer
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(hkpShapeContainer, m_pVftable, 0x0);
}
