#pragma once

#include <Marathon.inl>

namespace hk330
{
    class hkShapeCollectionFilter
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpCtor;
            be<uint32_t> fpIsCollisionEnabled;
        };

        xpointer<Vftable> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(hkShapeCollectionFilter, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(hkShapeCollectionFilter, 0x04);
}
