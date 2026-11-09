#pragma once

#include <Marathon.inl>

namespace hk330
{
    class hkRayShapeCollectionFilter
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpCtor;
            be<uint32_t> fpIsCollisionEnabled;
        };

        xpointer<Vftable> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(hkRayShapeCollectionFilter, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(hkRayShapeCollectionFilter, 0x04);
}
