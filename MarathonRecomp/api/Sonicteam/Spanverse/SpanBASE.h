#pragma once

#include <Marathon.inl>

namespace Sonicteam::Spanverse
{
    class SpanBASE
    {
    public:
        static const uint32_t Header = 0x42415345;

        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;
        be<uint32_t> m_Header; // BASE
        be<uint32_t> m_Field8;
        be<uint16_t> m_Flag;
    };

    MARATHON_ASSERT_SIZEOF(SpanBASE, 0x10);
}
