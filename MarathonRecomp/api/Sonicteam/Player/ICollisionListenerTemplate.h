#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ICollisionListener.h>

namespace Sonicteam::Player
{
    //Sonicteam::Player::ICollisionListener, Sonicteam::Player::ICollisionListenerTemplate<Sonicteam::SoX::Physics::IntersectListener,Sonicteam::SoX::Physics::IntersectEvent>
    
    template <typename Cast, typename Event>
    class ICollisionListenerTemplate:ICollisionListener,Cast
    {
    public:
   

    };
}
