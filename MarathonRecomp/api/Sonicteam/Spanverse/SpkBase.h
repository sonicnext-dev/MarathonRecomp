#pragma once

#include <Marathon.inl>

namespace Sonicteam::Spanverse
{
    struct SpkBaseDef;

    struct SpkBaseDef
    {
        be<uint32_t> fpCtor;
        be<uint32_t> ID;
        xpointer<SpkBaseDef> pSpkBaseDef;
    };

    class SpkBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x42415345; // BASE

        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;
        be<uint32_t> m_Signature;
        xpointer<SpkBaseDef> m_pDefInfo;
        be<uint32_t> m_CreationFlag;
    };

    MARATHON_ASSERT_SIZEOF(SpkBase, 0x10);
}
