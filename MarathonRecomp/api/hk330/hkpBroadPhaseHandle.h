#pragma once

#include <Marathon.inl>
#include <hk330/hkpShape.h>

namespace hk330
{
    class hkpBroadPhaseHandle
    {
    public:
        be<uint32_t> m_id;
    };

    MARATHON_ASSERT_OFFSETOF(hkpBroadPhaseHandle, m_id, 0x00);
}
