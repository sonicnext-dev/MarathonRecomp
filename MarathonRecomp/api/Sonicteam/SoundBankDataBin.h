#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoundBankData.h>
#include <stdx/compare/string_compare.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class SoundBankDataBin : public SoundBankData
    {
    public:
        xpointer<uint8_t> m_pResource;
        stdx::map<stdx::string, be<uint32_t>, stdx::string_compare> m_SoundIDs;
    };

    MARATHON_ASSERT_OFFSETOF(SoundBankDataBin, m_pResource, 0x18);
    MARATHON_ASSERT_OFFSETOF(SoundBankDataBin, m_SoundIDs, 0x1C);
    MARATHON_ASSERT_SIZEOF(SoundBankDataBin, 0x28);
}
