#pragma once

#include <Marathon.inl>
#include <hk330/hkpBroadPhaseHandle.h>

namespace hk330
{
    class hkpTypedBroadPhaseHandle : hkpBroadPhaseHandle
    {
    public: 
        int8_t m_type;
        int8_t m_owner;
        be<int16_t> m_QualityType;
        be<uint32_t> m_CollisionFilter;

    };
}

