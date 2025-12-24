#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkSpangleBase.h>

namespace Sonicteam::Spanverse
{
    class SpangleCamera : public SpkSpangleBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x5342434D; // SBCM
        MARATHON_INSERT_PADDING(0x30);
    };

    MARATHON_ASSERT_SIZEOF(SpangleCamera, 0xB0);
}
