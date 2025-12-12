#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPostureControl.h>
#include <Sonicteam/Player/IPostureSupportSphere.h>
#include <Sonicteam/Player/IPostureSupportOttoto.h>
#include <Sonicteam/Player/IPostureSupportEdge.h>
#include <Sonicteam/Player/IPostureSupportInput.h>
#include <Sonicteam/Player/IPostureSupportRayTemplate.h>
#include <Sonicteam/Player/GroundRayListener.h>
#include <Sonicteam/Player/IPosturePlugIn.h>
#include <Sonicteam/Player/PhysicsBodyList.h>
#include <Sonicteam/Player/RotationGravityAxis.h>
#include <Sonicteam/Player/MovableBody.h>
#include <Sonicteam/Player/PostureVehicle.h>
#include <Sonicteam/Player/PostureEdge.h>
#include <Sonicteam/SoX/Math/Math.h>

// DEBUGGING HFR ISSUES FOR PostureControl
#define MARATHON_POSTURECONTROL_USE_RECREATED_VERSION 0

namespace Sonicteam::Player
{
    class PostureControl : public IPostureControl, public IPostureSupportSphere, public IPostureSupportOttoto, public IPostureSupportEdge, public IPostureSupportInput, public IPostureSupportRayTemplate<GroundRayListener>
    {
    public:
        enum PostureFlag
        {
            // The player is detecting ground.
            PostureFlag_Grounded = 0x1,

            // The player is detecting death collision or water death.
            PostureFlag_DeathCollision = 0x2,

            // The player is brushing against a wall.
            PostureFlag_WallSide = 0x8,

            // The player is head on against a wall.
            PostureFlag_WallFront = 0x10,

            // The player is grinding on a rail.
            PostureFlag_RailGrind = 0x40,

            // The player is in the intermediate state between jumping and falling.
            PostureFlag_FallIntermediate = 0x100,

            // The player is falling.
            PostureFlag_Fall = 0x200,

            // The player is on water collision.
            PostureFlag_Water = 0x800,

            // The player is in the OTTOTTO state.
            PostureFlag_OTTOTTO = 0x2000,

            // The player is light dashing.
            PostureFlag_LightDash = 0x4000,

            // The player is rotating in a non-forward direction.
            PostureFlag_QuickRotate = 0x8000,

            // The player is on tentative collision.
            PostureFlag_Tentative = 0x10000,

            // The player is water sliding.
            PostureFlag_WaterSlide = 0x20000,

            // The player is on grass collision.
            PostureFlag_Grass = 0x100000,

            // The player is on dirt/clay collision.
            PostureFlag_DirtClay = 0x200000,

            // The player is on stone collision.
            PostureFlag_Stone = 0x400000,

            // The player is on material 1 collision.
            PostureFlag_Material1 = 0x800000,

            // The player is on shoreline collision.
            PostureFlag_Sand = 0x1000000,

            // The player is on material 2 collision.
            PostureFlag_Material2 = 0x2000000,

            // The player is on material 3 collision.
            PostureFlag_Material3 = 0x4000000,

            // The player is on material 4 collision.
            PostureFlag_Material4 = 0x8000000
        };

        enum ContactId
        {
            ContactId_Default = 0,
            ContactId_Path = 1,
            ContactId_WaterSlider = 2,
            ContactId_LightDash = 3,
            ContactId_PathCollision = 4,
            ContactId_Homing = 5,
            ContactId_6 = 6,
            ContactId_PostureVehicle = 8,
            ContactId_PostureRodeo = 9,
        };

        boost::shared_ptr<Sonicteam::Player::PhysicsBodyList> m_spPhysicsBodyList; 
        be<float> m_Weight;                                                        
        be<float> m_SlopeRad;                                                      
        be<float> m_SlopeRadB;                                                     
        be<float> m_DownForce;                                                     
        be<float> m_InterpGravity;                                                 
        be<uint32_t> m_PostureContinueNum;                                            
        be<float> m_PostureContinueLen;                                            
        be<float> m_RotationSpeedBorder;                                           
        be<float> m_PostureIntertiaMove;                                           
        boost::shared_ptr<Player::IPosturePlugIn> m_spPath;
        boost::shared_ptr<Player::IPosturePlugIn> m_spPathGuide;
        boost::shared_ptr<Player::IPosturePlugIn> m_spWaterSlider;
        boost::shared_ptr<Player::IPosturePlugIn> m_spPathLightDash;
        boost::shared_ptr<Player::IPosturePlugIn> m_spPathCollision;
        boost::shared_ptr<Player::IPosturePlugIn> m_spImpulse;
        boost::shared_ptr<Player::IPosturePlugIn> m_spMovableBody;
        boost::shared_ptr<Player::IPosturePlugIn> m_spRotationGravityAxis;
        boost::shared_ptr<Player::IPosturePlugIn> m_spHoming; 
        boost::shared_ptr<Player::IPosturePlugIn> m_spPostureEdge;
        boost::shared_ptr<Player::IPosturePlugIn> m_spPostureVehicle;
        boost::shared_ptr<Player::IPosturePlugIn> m_spPostureRodeo;
        SoX::Math::Vector m_VectorA0;
        SoX::Math::Vector m_GroundUp; // Inverted Normal
        bool m_DisableAccuratePosture;
        SoX::Math::Vector m_Vector3D0;
        SoX::Math::Vector m_Vector3E0;
        be<uint32_t> m_CurrentPostureContinueNum;
        MARATHON_INSERT_PADDING(0x8);
        
        RotationGravityAxis* GetRotationGravityAxis();

        MovableBody* GetMovableBody();

        uint32_t GetPostureContact();

        template <typename Type = Player::IPosturePlugIn>
        Type* GetPosturePath(uint32_t path);

        uint32_t OutPostureContactFlag(IPosturePlugIn* plugin);

        bool OutPostureContactPosition(IPosturePlugIn* plugin, SoX::Math::Vector* position);

        bool OutPostureContactUP(IPosturePlugIn* plugin, SoX::Math::Vector* position);

        bool sub_821F1858(IPosturePlugIn* plugin, SoX::Math::Vector vector, SoX::Math::Quaternion* quat);

        SoX::Math::Vector CalculateVerticalImpulse();

        bool sub_821F1410(uint32_t Flag1);

        bool CalculateForwardImpulse(SoX::Math::Vector& outForwardImpulse, uint32_t lostArgument, SoX::Math::Vector point,double delta);

        bool sub_821FC148(SoX::Math::Vector* lerpImpulse, SoX::Math::Vector forwardImpulse, SoX::Math::Vector VerticalImpulse, double delta);

        bool sub_821F1728(IPosturePlugIn* plugin, SoX::Math::Vector& impulse, SoX::Math::Vector& impulseForward, SoX::Math::Vector& impulseVertical, double delta);

        bool sub_821F18E0(IPosturePlugIn* plugin, SoX::Math::Vector contactNormal, SoX::Math::Quaternion* outGroundNormal);

        void sub_821FDF90(SoX::Math::Vector invertNormal);

        bool sub_821FE560(SoX::Math::Vector& position, SoX::Math::Vector& impulse, be<uint32_t>* postureFlag);

        void sub_821F1400(uint32_t inputFlag);

        void sub_821F1378(uint32_t inputFlag);

        void sub_821FEEF0(ICollisionListener* listener, uint32_t cflag);

        bool sub_821FF910(SoX::Math::Vector& position, SoX::Math::Quaternion* groundNormal, SoX::Math::Vector& impulse);

        bool sub_821FF0F0(SoX::Math::Vector& p1, SoX::Math::Quaternion* outNormal, SoX::Math::Vector p2);

        bool sub_821F17C8(IPosturePlugIn* plugin);

        bool sub_821FFC08(Sonicteam::SoX::Math::Vector& position, Sonicteam::SoX::Math::Quaternion* groundNormal, Sonicteam::SoX::Math::Vector& impulse);

        void sub_821F1968(SoX::Math::Vector v1, SoX::Math::Vector v2);

        void sub_821FDDB0();

        // Recreated from sub_82200538, Full 0.9:1 for debugging purposes
        void RecreatedUpdate(double deltaTime);
    };
	
    MARATHON_ASSERT_SIZEOF(PostureControl, 0x400);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spPhysicsBodyList, 0x314);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_Weight, 0x31C);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_SlopeRad, 0x320);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_SlopeRadB, 0x324);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_DownForce, 0x328);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_InterpGravity, 0x32C);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_PostureContinueNum, 0x330);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_PostureContinueLen, 0x334);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_RotationSpeedBorder, 0x338);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_PostureIntertiaMove, 0x33C);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spPath, 0x340);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spPathGuide, 0x348);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spWaterSlider, 0x350);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spPathLightDash, 0x358);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spPathCollision, 0x360);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spImpulse, 0x368);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spMovableBody, 0x370);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spRotationGravityAxis, 0x378);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spHoming, 0x380);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spPostureEdge, 0x388);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spPostureVehicle, 0x390);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_spPostureRodeo, 0x398);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_VectorA0, 0x3A0);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_GroundUp, 0x3B0);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_DisableAccuratePosture, 0x3C0);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_Vector3D0, 0x3D0);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_Vector3E0, 0x3E0);
    MARATHON_ASSERT_OFFSETOF(PostureControl, m_CurrentPostureContinueNum, 0x3F0);
}

#include "PostureControl.inl"
