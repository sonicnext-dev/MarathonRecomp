#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ICollisionListener.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/Entity.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::Player
{
    class TCast 
    {
        MARATHON_INSERT_PADDING(8);
    };

    class TEvent {};

    template <typename Cast, typename Event>
    class ICollisionListenerTemplate: public ICollisionListener, public Cast 
    {
    public:
        SoX::Math::Vector m_ContactPosition;
        SoX::Math::Vector m_ContactNormal;
        be<float> m_RayDistance; // Not sure
        be<uint32_t> m_RayFlag; // Not sure
        SoX::LinkRef<SoX::Physics::Entity> m_lrContactEntity;
    };

    typedef ICollisionListenerTemplate<TCast, TEvent> ICollisionListenerTemplate_Default;

    MARATHON_ASSERT_SIZEOF(ICollisionListenerTemplate_Default, 0x80);
    MARATHON_ASSERT_OFFSETOF(ICollisionListenerTemplate_Default, m_ContactPosition, 0x40);
    MARATHON_ASSERT_OFFSETOF(ICollisionListenerTemplate_Default, m_ContactNormal, 0x50);
    MARATHON_ASSERT_OFFSETOF(ICollisionListenerTemplate_Default, m_RayDistance, 0x60);
    MARATHON_ASSERT_OFFSETOF(ICollisionListenerTemplate_Default, m_RayFlag, 0x64);
    MARATHON_ASSERT_OFFSETOF(ICollisionListenerTemplate_Default, m_lrContactEntity, 0x68);
}   
