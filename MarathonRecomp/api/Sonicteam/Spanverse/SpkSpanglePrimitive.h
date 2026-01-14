#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkSpangleBase.h>

namespace Sonicteam::Spanverse
{
    class SpkSpanglePrimitive : public SpkSpangleBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x5342504D; // SBPM    
        MARATHON_INSERT_PADDING(0x140);
    };

    MARATHON_ASSERT_SIZEOF(SpkSpanglePrimitive, 0x1C0);
}
