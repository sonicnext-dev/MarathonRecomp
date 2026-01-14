#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkSpangleBase.h>

namespace Sonicteam::Spanverse
{
    class SpangleParticle : public SpkSpangleBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x53425043; // SBPC
        MARATHON_INSERT_PADDING(0x40);
    };

    MARATHON_ASSERT_SIZEOF(SpangleParticle, 0xC0);
}
