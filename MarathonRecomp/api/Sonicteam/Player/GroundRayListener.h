#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ICollisionListenerTemplate.h>
#include <Sonicteam/SoX/Physics/IntersectListener.h>
#include <Sonicteam/SoX/Physics/IntersectEvent.h>

namespace Sonicteam::Player
{
    class GroundRayListener : public ICollisionListenerTemplate<SoX::Physics::IntersectListener, SoX::Physics::IntersectEvent> {};

    MARATHON_ASSERT_SIZEOF(GroundRayListener, 0x80);
}
