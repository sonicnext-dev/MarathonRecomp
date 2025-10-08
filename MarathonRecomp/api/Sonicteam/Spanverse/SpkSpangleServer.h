#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkBase.h>
#include <Sonicteam/Spanverse/SpkEssenceBase.h>
#include <Sonicteam/Spanverse/SpkSpangleBase.h>

namespace Sonicteam::Spanverse
{
    // Hypthothetical name
    class SpanCSTM; // size = 0x24

    class SpkSpangleServer : public SpkBase
    {
    public:

        enum SpkSpangleBase
        {
            SpkSpangleBase_SpangleObject = 0,
            SpkSpangleBase_SpangleCellSprite = 4,
            SpkSpangleBase_SpangleCamera = 5,
            SpkSpangleBase_SpangleScreen = 6,
            SpkSpangleBase_SpangleLight = 7,
            SpkSpangleBase_SpanglePrimitive = 8,
            SpkSpangleBase_SpangleParticle = 9,     
        };


        static constexpr uint32_t ms_Signature = 0x53534753; // SSGS
        xpointer<SpanCSTM> m_pStanCSTM;
        xpointer<SpkSpangleBase> m_paSpkSpangleBase[5]; // SPOB (SpangleObject) SBSR (SpangleScreen) SBLI (SpangleLight) SBPM# (SpanglePrimitive) SpangleParticle (SBPC)
    };

    MARATHON_ASSERT_OFFSETOF(SpkSpangleServer, m_paSpkSpangleBase, 0x14);
}
