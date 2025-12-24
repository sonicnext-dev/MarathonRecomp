#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkSpangleBase.h>

namespace Sonicteam::Spanverse
{
    class SpangleCellSprite : public SpkSpangleBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x53424353; // SBCS
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_SIZEOF(SpangleCellSprite, 0x90);
}
