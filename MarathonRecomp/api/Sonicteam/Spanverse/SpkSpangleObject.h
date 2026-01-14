#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkSpangleBase.h>
#include <Sonicteam/SoX/Math/Matrix.h>

namespace Sonicteam::Spanverse
{
    class SpkSpangleObject : public SpkSpangleBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x5342504D; // SBPM
        MARATHON_INSERT_PADDING(0x30);  
    };

    MARATHON_ASSERT_OFFSETOF(SpkSpangleObject, m_Transform,0x10);
    MARATHON_ASSERT_SIZEOF(SpkSpangleObject, 0xB0);
}
