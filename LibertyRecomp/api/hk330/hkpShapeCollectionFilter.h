#pragma once

#include <Liberty.inl>

namespace hk330
{
    class hkpShapeCollectionFilter
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpCtor;
            be<uint32_t> fpIsCollisionEnabled;
        };

        xpointer<Vftable> m_pVftable;
    };

    LIBERTY_ASSERT_OFFSETOF(hkpShapeCollectionFilter, m_pVftable, 0x00);
}
