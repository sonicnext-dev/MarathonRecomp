#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkSpangleBase.h>

namespace Sonicteam::Spanverse
{
    class SpangleScreen : public SpkSpangleBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x53425352; // SBSR
        MARATHON_INSERT_PADDING(0x30);
    };

    MARATHON_ASSERT_SIZEOF(SpangleScreen, 0xB0);
}
