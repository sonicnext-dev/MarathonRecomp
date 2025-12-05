#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackMomBase.h>

namespace Sonicteam::Spanverse
{
    // constructor 0x12629a0
    // ID: 0x2
    class _acmTranslateAccel : public _ackMomBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x4D544158; // MTAX
        MARATHON_INSERT_PADDING(0x24);
    };

    MARATHON_ASSERT_SIZEOF(_acmTranslateAdd, 0x60);
}
