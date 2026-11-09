#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/PhysicsSystemFactory.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class PhysicsSystemFactoryHavok : public PhysicsSystemFactory {};

    MARATHON_ASSERT_SIZEOF(PhysicsSystemFactoryHavok, 0x04);
}
