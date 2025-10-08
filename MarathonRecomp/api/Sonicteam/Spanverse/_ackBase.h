#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkBase.h>

namespace Sonicteam::Spanverse
{
    // Constructor 828F8FD0
    class _ackBase : public SpkBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x41434253; // ACBS
    };

    MARATHON_ASSERT_SIZEOF(_ackBase, 0x10);
}
