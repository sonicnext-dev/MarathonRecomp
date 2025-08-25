#pragma once

#include <Marathon.inl>
#include <hk330/hkpCdBody.h>
#include <hk330/hkpTypedBroadPhaseHandle.h>

namespace hk330
{
    class hkpCollidable : hkpCdBody
    {
    public:
        MARATHON_INSERT_PADDING(0x4);
        hkpTypedBroadPhaseHandle m_BroadPhaseHandle;
        MARATHON_INSERT_PADDING(0x28);
        be<float> m_PenetrationDepth;
    };
}

