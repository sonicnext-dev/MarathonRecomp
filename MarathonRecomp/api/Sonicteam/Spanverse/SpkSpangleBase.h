#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkBase.h>
#include <Sonicteam/SoX/Math/Matrix.h>

namespace Sonicteam::Spanverse
{
    class SpkSpangleBase : public SpkBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x53505342; // SPSB

        SoX::Math::Matrix4x4 m_Transform;
        MARATHON_INSERT_PADDING(0x30);
    
    };

    MARATHON_ASSERT_SIZEOF(SpkSpangleBase, 0x80);
}
