#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ICollisionListenerTemplate.h>
#include <Sonicteam/SoX/Physics/IntersectListener.h>
#include <Sonicteam/SoX/Physics/IntersectEvent.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/Entity.h>
#include <Sonicteam/SoX/Physics/EntityRef.h>

namespace Sonicteam::Player
{
    //Sonicteam::Player::ICollisionListener, Sonicteam::Player::ICollisionListenerTemplate<Sonicteam::SoX::Physics::IntersectListener,Sonicteam::SoX::Physics::IntersectEvent>
    class GroundRayListener:public ICollisionListenerTemplate<Sonicteam::SoX::Physics::IntersectListener, Sonicteam::SoX::Physics::IntersectEvent>
    {
    public:
        SoX::Math::Vector m_ContactPosition;
        SoX::Math::Vector m_ContactNormal;
        be<float> m_RayDistance; //not sure
        be<uint32_t> m_RayFlag; //not sure
        SoX::Physics::EntityRef m_ContactEntity;
        MARATHON_INSERT_PADDING(0xC);
    };
}
