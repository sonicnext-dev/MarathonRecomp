#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IForce.h>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/IPosturePlugIn.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/Player/IPostureSupportRayTemplate.h>
#include <Sonicteam/Player/WallRayListener.h>
#include <Sonicteam/Player/GroundRayListener.h>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam::Player
{
    class PostureEdge : public IPlugIn, public IFlagCommunicator, public IPosturePlugIn, public IPostureSupportRayTemplate<Sonicteam::Player::WallRayListener>, public IPostureSupportRayTemplate<Sonicteam::Player::GroundRayListener>
    {
    public:
        SoX::RefSharedPointer<SoX::Physics::World> m_spPhysicsWorld;
        be<uint32_t> m_Field54;
        SoX::Math::Vector m_Vector60;
        bool m_Field70;
        be<uint32_t> m_Field74;
    };

    MARATHON_ASSERT_SIZEOF(PostureEdge, 0x80);
    MARATHON_ASSERT_OFFSETOF(PostureEdge, m_spPhysicsWorld, 0x50);
    MARATHON_ASSERT_OFFSETOF(PostureEdge, m_Field54, 0x54);
    MARATHON_ASSERT_OFFSETOF(PostureEdge, m_Vector60, 0x60);
    MARATHON_ASSERT_OFFSETOF(PostureEdge, m_Field70, 0x70);
    MARATHON_ASSERT_OFFSETOF(PostureEdge, m_Field74, 0x74);
}
