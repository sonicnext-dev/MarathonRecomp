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

    MARATHON_ASSERT_OFFSETOF(hkReferencedObject, m_pVftable, 0);
    MARATHON_ASSERT_OFFSETOF(hkReferencedObject, m_memSizeAndFlags, 4);
    MARATHON_ASSERT_OFFSETOF(hkReferencedObject, m_referenceCount, 6);
}
