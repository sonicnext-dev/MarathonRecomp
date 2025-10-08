#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkBase.h>

namespace Sonicteam::Spanverse
{
    class SpanSPMS; // _spkMatrixStack
    class SpanSPCM; // SpkCamera

    class SpkEnv : SpkBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x53505342; // SPSB
        xpointer<SpanSPMS> m_pSpanSPMS;
        xpointer<SpanSPCM> m_pSpanSPCM;
        MARATHON_INSERT_PADDING(0x88);
    };

    MARATHON_ASSERT_OFFSETOF(SpkEnv,m_pSpanSPMS, 0x10);
    MARATHON_ASSERT_OFFSETOF(SpkEnv,m_pSpanSPCM, 0x14);
    MARATHON_ASSERT_SIZEOF(SpkEnv, 0xA0);
}
