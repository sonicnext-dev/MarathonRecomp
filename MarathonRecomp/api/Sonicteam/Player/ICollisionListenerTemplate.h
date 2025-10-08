#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ICollisionListener.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/Entity.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::Player
{
    template <typename Cast, typename Event>
    class ICollisionListenerTemplate: public ICollisionListener, public Cast 
    {
        SoX::Math::Vector m_ContactPosition;
        SoX::Math::Vector m_ContactNormal;
        be<float> m_RayDistance; // Not sure
        be<uint32_t> m_RayFlag; // Not sure
        SoX::LinkRef<SoX::Physics::Entity> m_lrContactEntity;
    };

    //MARATHON_ASSERT_SIZEOF(ICollisionListenerTemplate<void,void>, 0x80);
    //MARATHON_ASSERT_OFFSETOF(ICollisionListenerTemplate<SoX::RefCountObject, EventTemplate>, m_ContactPosition, 0x40);
    //MARATHON_ASSERT_OFFSETOF(ICollisionListenerTemplate<SoX::RefCountObject, EventTemplate>, m_ContactNormal, 0x50);
    //MARATHON_ASSERT_OFFSETOF(ICollisionListenerTemplate<SoX::RefCountObject, EventTemplate>, m_RayDistance, 0x60);
    //MARATHON_ASSERT_OFFSETOF(ICollisionListenerTemplate<SoX::RefCountObject, EventTemplate>, m_RayFlag, 0x64);
    //MARATHON_ASSERT_OFFSETOF(ICollisionListenerTemplate<SoX::RefCountObject, EventTemplate>, m_ContactEntity, 0x68);
}   
