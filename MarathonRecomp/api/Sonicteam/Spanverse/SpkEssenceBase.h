#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkBase.h>

namespace Sonicteam::Spanverse
{
    class SpkEssenceBase : SpkBase
    {
    public:
        static constexpr uint32_t ms_Signature = 0x53504542; // SPEB
    };

    MARATHON_ASSERT_SIZEOF(SpkEssenceBase, 0x10);
}
