#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ICollisionListenerTemplate.h>
#include <Sonicteam/SoX/Physics/ShapeCastListener.h>
#include <Sonicteam/SoX/Physics/ShapeCastEvent.h>

namespace Sonicteam::Player
{
    class GroundListener: public ICollisionListenerTemplate<SoX::Physics::ShapeCastListener, SoX::Physics::ShapeCastEvent> {};

    MARATHON_ASSERT_SIZEOF(GroundListener, 0x80);
}
