#pragma once

#include <Marathon.inl>

namespace Sonicteam::Spanverse
{
    class SpanBASE
    {
    public:
        static constexpr uint32_t ms_Signature = 0x42415345; // BASE

        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;
        be<uint32_t> m_Signature;
        be<uint32_t> m_Field08;
        be<uint16_t> m_Flags;
    };

    MARATHON_ASSERT_SIZEOF(SpanBASE, 0x10);
}
