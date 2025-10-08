#pragma once

#include <Marathon.inl>

#include <Sonicteam/ActorManager.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Math/Math.h>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/State/ICommonContextIF.h>
#include <Sonicteam/Player/IDynamicLink.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/Player/IPosturePlugIn.h>
#include <Sonicteam/Player/Unit/ITestCase.h>
#include <Sonicteam/Player/IForce.h>
#include <Sonicteam/Player/Input/IListener.h>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::Player
{
    class IPostureControl : public IVariable, public IDynamicLink, public Unit::ITestCase, public IFlagCommunicator
    {
    public:
        struct Vftable : IVariable::Vftable
        {
            be<uint32_t> fpfunc4;
            be<uint32_t> fpStep;
            be<uint32_t> fpImportPad; //+-
        };

        // Hypthothecil, based on Context 
        enum ContextIFFlag
        {
            ContextIFFlag_IsSonicDead = 0x1,                    // From v17 calculation
            ContextIFFlag_BlinkInvulMode = 0x2,                 // Blink invulnerability mode
            ContextIFFlag_InvulnerableState = 0x4,              // Combined invulnerability state
            ContextIFFlag_AnimationRotationLocked = 0x8,        // IsAnimationRotationLocked
            ContextIFFlag_IsUnkE6 = 0x10,                       // Unknown flag E6
            ContextIFFlag_GravityDisabled = 0x20,               // IsGravityDisabled or ResetGravity
            ContextIFFlag_JumpedPosture = 0x40,                 // IsJumped_PostureVelocityYHandle
            ContextIFFlag_InActionsRunJumpBound = 0x80,         // IsInActionsRunJumpBound_MB
            ContextIFFlag_JumpPush = 0x1000,                    // JumpPush
            ContextIFFlag_IsUnkEC = 0x200,                      // Unknown flag EC
            ContextIFFlag_IsUnkED = 0x400,                      // Unknown flag ED
            ContextIFFlag_IsUnkEE = 0x800,                      // Unknown flag EE
            ContextIFFlag_DamageInvincibleActive = 0x2000,      // DamageInvincibleTime > 0.0
            ContextIFFlag_UnkF0 = 0x4000,                       // Unknown flag F0
            ContextIFFlag_UnkF1 = 0x8000,                       // Unknown flag F1
            ContextIFFlag_UnkF2 = 0x10000,                      // Unknown flag F2
            ContextIFFlag_UnkF3 = 0x20000,                      // Unknown flag F3
            ContextIFFlag_EdgeGrab = 0x40000,                   // IsEdgeGrab
            ContextIFFlag_WeaponRequest = 0x80000,              // WeaponRequestFlag related
            ContextIFFlag_DisableGravity = 0x200000,            // DisableGravity
            ContextIFFlag_IsUnkFE = 0x400000,                   // Unknown flag FE
            ContextIFFlag_IsUnkFF = 0x800000,                   // Unknown flag FF
            ContextIFFlag_IsUnk107 = 0x1000000,                 // Unknown flag 107
            ContextIFFlag_InvulnerableItemBox = 0x8000000,      // IsInvulnerableItemBox
            ContextIFFlag_GrabbedAura = 0x2000000,              // IsGrabbedAura
            ContextIFFlag_GhostMode = 0x4000000,                // IsGhostMode
            ContextIFFlag_InputLocked = 0x8,                    // LockInputTime > 0.0 (duplicate bit, appears to be OR'd)
            ContextIFFlag_InputLockedAlt = 0x100,               // LockInputTime > 0.0 (alternative bit position)
        };

        SoX::RefSharedPointer<RootFrame> m_spRootFrame;
        SoX::Math::Quaternion m_LocalRotation;
        SoX::Math::Vector m_LocalPosition;
        SoX::RefSharedPointer<SoX::Physics::World> m_spWorld;
        boost::shared_ptr<IForce> m_spGravityForce;
        boost::shared_ptr<Input::IListener> m_spInputListener;
        boost::shared_ptr<Input::IListener> m_spAmigoListener;
        boost::shared_ptr<State::ICommonContextIF> m_ContextIF;
        boost::shared_ptr<ActorManager> m_spActorManager;
        xpointer<SoX::Engine::Task> m_pPlayerTask; // Object::Player, but as ::Task
        boost::shared_ptr<IPosturePlugIn> m_spPosturePlugIn;
        SoX::Math::Vector m_GravityDirection;
        be<float> m_GravityForce;
        SoX::Math::Quaternion m_GroundNormal; // GroundRotation??
        SoX::Math::Vector m_Position;
        SoX::Math::Quaternion m_Rotation;
        SoX::Math::Quaternion m_PadRotation;
        MARATHON_INSERT_PADDING(0x10);
        be<uint32_t> m_PostureFlag;
        be<float> m_ImpulseForward;
        be<float> m_ImpulseVertical;
        SoX::Math::Vector m_ImpulseUP;
        be<uint32_t> m_ContextIFFlag;
        be<uint32_t> m_PostureRequestFlag;
        be<uint32_t> m_PostureFlag118;
        be<uint32_t> m_PostureFlag11C;

        template <typename T = SoX::Physics::World>
        T* GetWorld();

        template <typename T = IForce>
        T* GetGravity();

        SoX::Math::Quaternion CalculateDefaultTaskPad();

        void ImportPad(SoX::Math::Quaternion pad, float step);

        // Recreate from sub_821F1E30, Full 1:1 recreated for debugging purposes
        void RecreatedUpdate(double deltaTime);
    };

    MARATHON_ASSERT_SIZEOF(IPostureControl, 0x120);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spRootFrame, 0x10);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_LocalRotation, 0x20);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_LocalPosition, 0x30);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spWorld, 0x40);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spGravityForce, 0x44);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spInputListener, 0x4C);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spAmigoListener, 0x54);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_ContextIF, 0x5C);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spActorManager, 0x64);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_pPlayerTask, 0x6C);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_spPosturePlugIn, 0x70);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_GravityDirection, 0x80);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_GravityForce, 0x90);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_GroundNormal, 0xA0);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_Position, 0xB0);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_Rotation, 0xC0);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_PadRotation, 0xD0);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_PostureFlag, 0xF0);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_ImpulseForward, 0xF4);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_ImpulseVertical, 0xF8);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_ImpulseUP, 0x100);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_ContextIFFlag, 0x110);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_PostureRequestFlag, 0x114);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_PostureFlag118, 0x118);
    MARATHON_ASSERT_OFFSETOF(IPostureControl, m_PostureFlag11C, 0x11C);
}


#include "IPostureControl.inl"
