#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/State/ICommonContextIF.h>
#include <Sonicteam/Player/IDynamicLink.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/Player/IPosturePlugIn.h>
#include <Sonicteam/Player/Unit/ITestCase.h>
#include <Sonicteam/SoX/RefCountObject.h>
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
        SoX::RefSharedPointer<SoX::RefCountObject> m_spRootFrame;
        SoX::Math::Quaternion m_RotationFixed;
        SoX::Math::Vector m_PositionFixed;
        SoX::RefSharedPointer<SoX::Physics::World> m_spWorld;
        boost::shared_ptr<void> m_spGravity;
        boost::shared_ptr<void> m_spInputListener;
        boost::shared_ptr<void> m_spAmigoListener;
        boost::shared_ptr<State::ICommonContextIF> m_ContextIF;
        boost::shared_ptr<ActorManager> m_spActorManager;
        xpointer<SoX::Engine::Task> m_pTask;
        boost::shared_ptr<IPosturePlugIn> m_spPosturePlugIn;
        SoX::Math::Vector m_GravityDirection;
        be<float> m_GravityForce;
        SoX::Math::Vector m_NormalizedSurface; //ground normal?
        SoX::Math::Vector m_Position;
        SoX::Math::Quaternion m_Rotation;
        MARATHON_INSERT_PADDING(0x20);
        be<uint32_t> m_PostureFlag;
        be<float> m_ImpulseForward;
        be<float> m_ImpulseVertical;
        SoX::Math::Vector m_ImpulseUP;
        be<uint32_t> m_ContextIFFlag;
        be<uint32_t> m_PostureRequestFlag;
        be<uint32_t> m_PostureFlag118;
        be<uint32_t> m_PostureFlag11C;
    };
}
