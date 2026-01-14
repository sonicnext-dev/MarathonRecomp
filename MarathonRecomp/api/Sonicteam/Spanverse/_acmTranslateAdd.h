#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackMomBase.h>

namespace Sonicteam::Spanverse
{
    // constructor 0x1262e14
    // ID: 0x1
    class _acmTranslateAdd : public _ackMomBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x4D544144; // MTAD
        MARATHON_INSERT_PADDING(0x24);
    };

    MARATHON_ASSERT_SIZEOF(_acmTranslateAdd, 0x60);
}
