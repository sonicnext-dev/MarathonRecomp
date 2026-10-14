#pragma once

#include <Marathon.inl>

namespace Chao::CSD
{
    template <typename T>
    class RCObject
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<T> m_pObject;
        be<uint32_t> m_ReferenceCount;
        xpointer<void> m_Field0C;
        be<uint32_t> m_Field10;
    };

    MARATHON_ASSERT_OFFSETOF(RCObject<void>, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(RCObject<void>, m_pObject, 0x04);
    MARATHON_ASSERT_OFFSETOF(RCObject<void>, m_ReferenceCount, 0x08);
    MARATHON_ASSERT_OFFSETOF(RCObject<void>, m_Field0C, 0x0C);
    MARATHON_ASSERT_OFFSETOF(RCObject<void>, m_Field10, 0x10);
    MARATHON_ASSERT_SIZEOF(RCObject<void>, 0x14);
}
