#pragma once

#include <Liberty.inl>
#include <hk330/hkpShape.h>

namespace hk330
{
    class hkpBroadPhaseHandle
    {
    public:
        be<uint32_t> m_id;
    };

    LIBERTY_ASSERT_OFFSETOF(hkpBroadPhaseHandle, m_id, 0x00);
}
