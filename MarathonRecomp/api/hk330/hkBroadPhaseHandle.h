#pragma once

#include <Marathon.inl>

namespace hk330
{
    class hkBroadPhaseHandle
    {
    public:
        be<uint32_t> m_id;
    };

    MARATHON_ASSERT_OFFSETOF(hkBroadPhaseHandle, m_id, 0x00);
    MARATHON_ASSERT_SIZEOF(hkBroadPhaseHandle, 0x04);
}
