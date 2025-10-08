#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackMomBase.h>

namespace Sonicteam::Spanverse
{
    // constructor 0x1264340
    // ID: 0x0
    class _acmTranslateNormal : public _ackMomBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x4D544E4F; // MTNO
    };

    MARATHON_ASSERT_SIZEOF(_acmTranslateNormal, 0x3C);
}
