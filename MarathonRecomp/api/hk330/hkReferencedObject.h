#pragma once

#include <Marathon.inl>

namespace hk330
{
    class hkReferencedObject
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDtor;
        };

        xpointer<Vftable> m_pVftable;
        be<uint16_t> m_memSizeAndFlags{};
        be<uint16_t> m_referenceCount{};
    };
}
