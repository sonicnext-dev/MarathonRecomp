#pragma once

#include <Marathon.inl>

namespace Chao::CSD
{
    class CMemoryAlloc
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDtor;
            be<uint32_t> fpAlloc;
            be<uint32_t> fpFree;
            MARATHON_INSERT_PADDING(0x10);
        };

        xpointer<Vftable> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(CMemoryAlloc::Vftable, fpDtor, 0x00);
    MARATHON_ASSERT_OFFSETOF(CMemoryAlloc::Vftable, fpAlloc, 0x04);
    MARATHON_ASSERT_OFFSETOF(CMemoryAlloc::Vftable, fpFree, 0x08);
    MARATHON_ASSERT_SIZEOF(CMemoryAlloc::Vftable, 0x1C);

    MARATHON_ASSERT_OFFSETOF(CMemoryAlloc, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(CMemoryAlloc, 0x04);
}
