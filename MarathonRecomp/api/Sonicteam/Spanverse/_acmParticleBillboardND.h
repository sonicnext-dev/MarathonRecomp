#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackMomBase.h>

namespace Sonicteam::Spanverse
{
    // constructor 0x1243b6c
    // ID: 0x6A
    class _acmParticleBillboardND : public _ackMomBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x00000003; // 
        MARATHON_INSERT_PADDING(0xB4);
    };

    MARATHON_ASSERT_SIZEOF(_acmParticleBillboardND, 0xF0);
}