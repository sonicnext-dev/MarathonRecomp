#pragma once

#include <Chao/CSD/Core/csdRCPtr.h>

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
        MARATHON_INSERT_PADDING(4);
        xpointer<T> m_pResource;
    };

    MARATHON_ASSERT_OFFSETOF(CResourceBase<void>, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(CResourceBase<void>, m_pResource, 0x08);
}
