#pragma once

namespace Chao::CSD
{
    template<typename T>
    class CResourceBase
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpCopyResource;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<RCObject<unsigned char>> m_pResourceData;
        xpointer<T> m_pResource;
    };

    MARATHON_ASSERT_OFFSETOF(CResourceBase<void>, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(CResourceBase<void>, m_pResourceData, 0x04);
    MARATHON_ASSERT_OFFSETOF(CResourceBase<void>, m_pResource, 0x08);
}
