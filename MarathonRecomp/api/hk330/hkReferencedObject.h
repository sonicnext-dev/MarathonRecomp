#pragma once

#include <Marathon.inl>

namespace hk330
{
    class hkReferencedObject
    {
    public:
        xpointer<void> m_pVftable;
        be<uint16_t> m_memSizeAndFlags{};
        be<uint16_t> m_referenceCount{};
    };
}
