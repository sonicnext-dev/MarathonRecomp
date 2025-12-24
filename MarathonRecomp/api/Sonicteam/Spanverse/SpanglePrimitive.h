#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkSpanglePrimitive.h>

namespace Sonicteam::Spanverse
{
    class SpanglePrimitive : public SpkSpanglePrimitive
    {
    public:
        // NO CUSTOM SIGNATURE
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_SIZEOF(SpanglePrimitive, 0x1D0);
}
