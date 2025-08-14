#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/World.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class WorldHavok : public World
    {
    public:
        MARATHON_INSERT_PADDING(0x18);
        bool m_IsDynamicUpdateRate;
    };
}
