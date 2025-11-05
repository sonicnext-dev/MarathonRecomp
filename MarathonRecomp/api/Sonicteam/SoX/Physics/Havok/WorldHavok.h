#pragma once

#include <Marathon.inl>
#include <hk330/hkpWorld.h>
#include <Sonicteam/SoX/Physics/World.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class WorldHavok : public World
    {
    public:
        xpointer<hk330::hkpWorld> m_pWorld;
        MARATHON_INSERT_PADDING(0x14);
        bool m_IsDynamicUpdateRate;
    };

    MARATHON_ASSERT_OFFSETOF(WorldHavok, m_pWorld, 0x08);
    MARATHON_ASSERT_OFFSETOF(WorldHavok, m_IsDynamicUpdateRate, 0x20);
    MARATHON_ASSERT_SIZEOF(WorldHavok, 0x24);
}
