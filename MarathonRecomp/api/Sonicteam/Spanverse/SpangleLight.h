#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkSpangleBase.h>

namespace Sonicteam::Spanverse
{
    class SpangleLight : public SpkSpangleBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x53424C49; // SBLI
        MARATHON_INSERT_PADDING(0x40);
    };

    MARATHON_ASSERT_SIZEOF(SpangleLight, 0xC0);
}
