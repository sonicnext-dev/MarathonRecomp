#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackMomBase.h>

namespace Sonicteam::Spanverse
{
    // constructor 0x123dc50
    // ID: 0x1F4
    class _acmMotionSet : public _ackMomBase
    {
    public:
        //static constexpr uint32_t ms_Signature = 0xFFFFFFFFFFFFFFFF;
        MARATHON_INSERT_PADDING(0x44);
    };

    MARATHON_ASSERT_SIZEOF(_acmMotionSet, 0x80);
}
