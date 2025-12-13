#pragma once

#include <Liberty.inl>

namespace hk330
{
    class hkReferencedObject
    {
    public:
        xpointer<void> m_pVftable;
        be<uint16_t> m_memSizeAndFlags;
        be<uint16_t> m_referenceCount;
    };

    LIBERTY_ASSERT_OFFSETOF(hkReferencedObject, m_pVftable, 0x00);
    LIBERTY_ASSERT_OFFSETOF(hkReferencedObject, m_memSizeAndFlags, 0x04);
    LIBERTY_ASSERT_OFFSETOF(hkReferencedObject, m_referenceCount, 0x06);
}
