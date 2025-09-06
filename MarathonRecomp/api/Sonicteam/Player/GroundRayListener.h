#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    //Sonicteam::Player::ICollisionListener, Sonicteam::Player::ICollisionListenerTemplate<Sonicteam::SoX::Physics::IntersectListener,Sonicteam::SoX::Physics::IntersectEvent>
    class GroundRayListener
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x7C);
    };
}
