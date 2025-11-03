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
    };

    MARATHON_ASSERT_OFFSETOF(GroundRayListener, m_ContactPosition, 0x50);
    MARATHON_ASSERT_OFFSETOF(GroundRayListener, m_ContactNormal, 0x60);
    MARATHON_ASSERT_OFFSETOF(GroundRayListener, m_RayDistance, 0x70);
    MARATHON_ASSERT_OFFSETOF(GroundRayListener, m_RayFlags, 0x74);
    MARATHON_ASSERT_OFFSETOF(GroundRayListener, m_ContactEntity, 0x78);
    MARATHON_ASSERT_SIZEOF(GroundRayListener, 0x90);
}
