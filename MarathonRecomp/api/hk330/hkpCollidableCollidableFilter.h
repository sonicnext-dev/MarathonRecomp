#pragma once

#include <Marathon.inl>

namespace hk330
{
    class hkpCollidableCollidableFilter
    {
    public:
        struct Vftable
        {
            be<uint32_t> ctor;
            be<uint32_t> isCollisionEnabled; //(a,b)
        };

        xpointer<Vftable> m_pVftable;
    };
}
