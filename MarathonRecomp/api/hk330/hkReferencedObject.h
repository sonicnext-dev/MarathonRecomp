#pragma once

#include <Marathon.inl>

namespace hk330
{
    class hkReferencedObject
    {
        xpointer<void> m_pVftable;
        be<uint16_t> m_memSizeAndFlags{};
        be<uint16_t> m_referenceCount{};
    };
}

