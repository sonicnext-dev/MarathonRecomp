#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPosturePlugIn.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <api/boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Physics/Body.h>

namespace Sonicteam::Player
{
    class PhysicsBodyList;

    class MovableBody : public IPosturePlugIn
    {
    public:
        boost::shared_ptr<PhysicsBodyList> m_spPhysicsBodyList;
        SoX::LinkRef<SoX::Physics::Body> m_lrBody;
        SoX::Math::Vector m_BodyForce;
    };

    MARATHON_ASSERT_SIZEOF(MovableBody, 0x30);
    MARATHON_ASSERT_OFFSETOF(MovableBody, m_spPhysicsBodyList, 0x4);
    MARATHON_ASSERT_OFFSETOF(MovableBody, m_lrBody, 0xC);
    MARATHON_ASSERT_OFFSETOF(MovableBody, m_lrBody, 0xC);
}
