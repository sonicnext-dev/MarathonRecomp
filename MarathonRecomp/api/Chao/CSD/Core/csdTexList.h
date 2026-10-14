#pragma once

#include <Chao/CSD/Core/csdBase.h>

namespace Chao::CSD
{
    class CTexList : public CBase
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<uint8_t> m_pData;
    };

    MARATHON_ASSERT_OFFSETOF(CTexList, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(CTexList, m_pData, 0x04);
}
