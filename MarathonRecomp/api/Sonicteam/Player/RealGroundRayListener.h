#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ICollisionListenerTemplate.h>
#include <Sonicteam/SoX/Physics/ShapeCastListener.h>
#include <Sonicteam/SoX/Physics/ShapeCastEvent.h>

namespace Sonicteam::Player
{
    class RealGroundRayListener: public ICollisionListenerTemplate<SoX::Physics::ShapeCastListener, SoX::Physics::ShapeCastEvent> {};

    MARATHON_ASSERT_SIZEOF(RealGroundRayListener, 0x80);
}
