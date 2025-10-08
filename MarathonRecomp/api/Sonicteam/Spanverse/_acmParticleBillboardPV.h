#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackMomBase.h>

namespace Sonicteam::Spanverse
{
    // constructor 0x1246978
    // ID: 0x67
    class _acmParticleBillboardPV : public _ackMomBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x00000003; // 
        MARATHON_INSERT_PADDING(0xA4);
    };

    MARATHON_ASSERT_SIZEOF(_acmParticleBillboardPV, 0xE0);
}