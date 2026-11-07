#pragma once

#include <Chao/CSD/Core/csdBase.h>
#include <Chao/CSD/Core/csdPlatform.h>

namespace Chao::CSD
{
    class CTexList : public CBase
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDtor;
            be<uint32_t> fpGetSignature;
            be<uint32_t> fpCreate;
            MARATHON_INSERT_PADDING(0x04);
            be<uint32_t> fpSetTexture;
            be<uint32_t> fpSetSamplerFilter;
            be<uint32_t> fpSetSamplerAddressMode;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<uint8_t> m_pData;
        xpointer<CPlatform> m_pPlatform;
        MARATHON_INSERT_PADDING(0x14);
    };

    MARATHON_ASSERT_OFFSETOF(CTexList, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(CTexList, m_pData, 0x04);
    MARATHON_ASSERT_OFFSETOF(CTexList, m_pPlatform, 0x08);
    MARATHON_ASSERT_SIZEOF(CTexList, 0x20);
}
