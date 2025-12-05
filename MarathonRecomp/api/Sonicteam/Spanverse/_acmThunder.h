#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackMomBase.h>

namespace Sonicteam::Spanverse
{
    // constructor 0x1260f4c
    // ID: 0x258
    class _acmThunder : public _ackMomBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x00000001; // 
        MARATHON_INSERT_PADDING(0x174);
    };

    MARATHON_ASSERT_SIZEOF(_acmThunder, 0x1B0);
}