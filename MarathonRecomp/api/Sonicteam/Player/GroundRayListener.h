#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ICollisionListenerTemplate.h>
#include <Sonicteam/SoX/Physics/IntersectListener.h>
#include <Sonicteam/SoX/Physics/IntersectEvent.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/Entity.h>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::Player
{
    class GroundRayListener : public ICollisionListenerTemplate<SoX::Physics::IntersectListener, SoX::Physics::IntersectEvent>
    {
    public:
        SoX::Math::Vector m_ContactPosition;
        SoX::Math::Vector m_ContactNormal;
        be<float> m_RayDistance;
        be<uint32_t> m_RayFlags;
        SoX::LinkRef<SoX::Physics::Entity> m_ContactEntity;
        MARATHON_INSERT_PADDING(0x0C);
    };
}
