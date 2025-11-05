#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class ICollisionListener
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x0C);
        be<uint32_t> m_FlagsA;
        be<uint32_t> m_FlagsB;
        be<uint32_t> m_FlagsC;
        MARATHON_INSERT_PADDING(4);
        SoX::Math::Vector m_SurfaceNormal;
    };

    MARATHON_ASSERT_OFFSETOF(ICollisionListener, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(ICollisionListener, m_FlagsA, 0x10);
    MARATHON_ASSERT_OFFSETOF(ICollisionListener, m_FlagsB, 0x14);
    MARATHON_ASSERT_OFFSETOF(ICollisionListener, m_FlagsC, 0x18);
    MARATHON_ASSERT_OFFSETOF(ICollisionListener, m_SurfaceNormal, 0x20);
}
