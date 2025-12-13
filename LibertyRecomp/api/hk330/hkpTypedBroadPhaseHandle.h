#pragma once

#include <Marathon.inl>
#include <hk330/hkpBroadPhaseHandle.h>

namespace hk330
{
    class hkpTypedBroadPhaseHandle : public hkpBroadPhaseHandle
    {
    public: 
        int8_t m_type;
        int8_t m_ownerOffset;
        be<int16_t> m_objectQualityType;
        be<uint32_t> m_collisionFilterInfo;
    };

    MARATHON_ASSERT_OFFSETOF(hkpTypedBroadPhaseHandle, m_type, 0x04);
    MARATHON_ASSERT_OFFSETOF(hkpTypedBroadPhaseHandle, m_ownerOffset, 0x05);
    MARATHON_ASSERT_OFFSETOF(hkpTypedBroadPhaseHandle, m_objectQualityType, 0x06);
    MARATHON_ASSERT_OFFSETOF(hkpTypedBroadPhaseHandle, m_collisionFilterInfo, 0x08);
}
