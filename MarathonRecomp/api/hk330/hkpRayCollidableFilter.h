#pragma once

#include <Marathon.inl>

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

    MARATHON_ASSERT_OFFSETOF(hkpRayCollidableFilter, m_pVftable, 0x00);
}
