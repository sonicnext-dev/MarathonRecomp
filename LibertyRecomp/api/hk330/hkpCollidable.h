#pragma once

#include <Liberty.inl>
#include <hk330/hkpCdBody.h>
#include <hk330/hkpTypedBroadPhaseHandle.h>

namespace hk330
{
    class hkpCollidable : public hkpCdBody
    {
    public:
        LIBERTY_INSERT_PADDING(4);
        hkpTypedBroadPhaseHandle m_broadPhaseHandle;
        LIBERTY_INSERT_PADDING(0x28);
        be<float> m_allowedPenetrationDepth;
    };

    LIBERTY_ASSERT_OFFSETOF(hkpCollidable, m_broadPhaseHandle, 0x14);
    LIBERTY_ASSERT_OFFSETOF(hkpCollidable, m_allowedPenetrationDepth, 0x48);
}
