#pragma once

#include <Liberty.inl>

namespace hk330
{
    class hkpRayShapeCollectionFilter
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpCtor;
            be<uint32_t> fpIsCollisionEnabled;
        };

        xpointer<Vftable> m_pVftable;
    };

    LIBERTY_ASSERT_OFFSETOF(hkpRayShapeCollectionFilter, m_pVftable, 0x00);
}
