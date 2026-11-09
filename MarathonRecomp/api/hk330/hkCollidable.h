#pragma once

#include <Marathon.inl>
#include <hk330/hkCdBody.h>
#include <hk330/hkTypedBroadPhaseHandle.h>

namespace hk330
{
    class hkCollidable : public hkCdBody
    {
    public:
        MARATHON_INSERT_PADDING(0x04);
        hkTypedBroadPhaseHandle m_broadPhaseHandle;
        MARATHON_INSERT_PADDING(0x28);
        be<float> m_allowedPenetrationDepth;
    };

    MARATHON_ASSERT_OFFSETOF(hkCollidable, m_broadPhaseHandle, 0x14);
    MARATHON_ASSERT_OFFSETOF(hkCollidable, m_allowedPenetrationDepth, 0x48);
}
