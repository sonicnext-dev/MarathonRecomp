#pragma once

#include <Liberty.inl>

namespace hk330
{
    class hkpRayCollidableFilter
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpCtor;
            be<uint32_t> fpIsCollisionEnabled;
        };

        xpointer<Vftable> m_pVftable;
    };

    LIBERTY_ASSERT_OFFSETOF(hkpRayCollidableFilter, m_pVftable, 0x00);
}
