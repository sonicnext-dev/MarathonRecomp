#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/State/ICommonContextIF.h>
#include <Sonicteam/Player/IDynamicLink.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/Player/IPosturePlugIn.h>
#include <Sonicteam/Player/Unit/ITestCase.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/Math/Quaternion.h>
#include <Sonicteam/SoX/Physics/World.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/ActorManager.h>

namespace Sonicteam::Player
{
    class IPostureControl : public IVariable, public IDynamicLink, public Unit::ITestCase, public IFlagCommunicator
    {
    public:
        SoX::RefSharedPointer<> m_spRootFrame;
        SoX::Math::Quaternion m_RotationFixed;
        SoX::Math::Vector m_PositionFixed;
        SoX::RefSharedPointer<SoX::Physics::World> m_spWorld;
        boost::shared_ptr<void> m_spGravity;
        boost::shared_ptr<void> m_spInputListener;
        boost::shared_ptr<void> m_spAmigoListener;
        boost::shared_ptr<State::ICommonContextIF> m_spCommonContextIF;
        boost::shared_ptr<ActorManager> m_spActorManager;
        xpointer<SoX::Engine::Task> m_pTask;
        boost::shared_ptr<IPosturePlugIn> m_spPosturePlugIn;
        SoX::Math::Vector m_GravityDirection;
        be<float> m_GravityForce;
        SoX::Math::Vector m_SurfaceNormal;
        SoX::Math::Vector m_Position;
        SoX::Math::Quaternion m_Rotation;
        MARATHON_INSERT_PADDING(0x20);
        be<uint32_t> m_PostureFlag;
        be<float> m_ImpulseForward;
        be<float> m_ImpulseVertical;
        SoX::Math::Vector m_ImpulseUp;
        be<uint32_t> m_CommonContextIFFlags;
        be<uint32_t> m_PostureRequestFlags;
        be<uint32_t> m_PostureFlags118;
        be<uint32_t> m_PostureFlags11C;
    };

    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spRootFrame, 0x10);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_RotationFixed, 0x20);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_PositionFixed, 0x30);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spWorld, 0x40);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spGravity, 0x44);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spInputListener, 0x4C);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spAmigoListener, 0x54);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spCommonContextIF, 0x5C);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spActorManager, 0x64);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_pTask, 0x6C);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spPosturePlugIn, 0x70);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_GravityDirection, 0x80);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_GravityForce, 0x90);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_SurfaceNormal, 0xA0);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_Position, 0xB0);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_Rotation, 0xC0);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_PostureFlag, 0xF0);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_ImpulseForward, 0xF4);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_ImpulseVertical, 0xF8);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_ImpulseUp, 0x100);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_CommonContextIFFlags, 0x110);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_PostureRequestFlags, 0x114);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_PostureFlags118, 0x118);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_PostureFlags11C, 0x11C);
    MARATHON_ASSERT_SIZEOF(IPostureControl, 0x120);
}
