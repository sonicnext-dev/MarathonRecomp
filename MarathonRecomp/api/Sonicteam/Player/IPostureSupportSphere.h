#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/SoX/Physics/RayCast.h>
#include <Sonicteam/SoX/Physics/ShapeCast.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/Player/GroundListener.h>
#include <Sonicteam/Player/RealGroundRayListener.h>
#include <Sonicteam/Player/WallListener.h>
#include <Sonicteam/Player/AllListener.h>
#include <Sonicteam/Player/WallRayListener.h>

namespace Sonicteam::Player
{
    class IPostureSupportSphere
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0xC);
        SoX::RefSharedPointer<SoX::Physics::World> m_spPhysicsWorld;                        
        boost::shared_ptr<Player::GroundListener> m_spGroundListener;                       
        boost::shared_ptr<Player::WallListener> m_spWallListener;                           
        boost::shared_ptr<Player::AllListener> m_spAllListener1;                            
        boost::shared_ptr<Player::AllListener> m_spAllListener2;                            
        boost::shared_ptr<Player::GroundRayListener> m_spGroundRayListener;                 
        boost::shared_ptr<Player::RealGroundRayListener> m_spRealGroundRayListener;         
        boost::shared_ptr<Player::WallRayListener> m_spWallRayListener;                     
        //Array[2] [0] - Normal [1] - Small
        SoX::Array<SoX::RefSharedPointer<SoX::Physics::ShapeCast>,2> m_aShapeCollisionAll;  
        SoX::Array<SoX::RefSharedPointer<SoX::Physics::ShapeCast>,2> m_aShapeCollisionWalk; 
        SoX::Array<SoX::RefSharedPointer<SoX::Physics::ShapeCast>,2> m_aShapeCollisionWall; 
        SoX::RefSharedPointer<SoX::Physics::RayCast> m_RayCast;                             
        xpointer<be<uint32_t>> m_pPostureRequestFlag;                                       
        SoX::Math::Vector m_CollisionAllAngle;                                              
        SoX::Array<be<float>, 2> m_aCollisionAllRadius;                                     
        SoX::Math::Vector m_CollisionWalkAngle;                                             
        SoX::Array<be<float>, 2> m_aCollisionWalkRadius;                                    
        SoX::Math::Vector m_CollisionWallAngle;                                             
        SoX::Array<be<float>, 2> m_aCollisionWallRadius;                                    
        be<uint32_t> m_FlagC8; //m_Normal, Small Toggle, not sure if bool either?
            
        Player::GroundListener* sub_822014B8(SoX::Math::Vector point);

        ICollisionListener* sub_82202180(SoX::Math::Vector position, SoX::Math::Vector endPosition, be<uint32_t>* flag);

        void sub_82200EC0(SoX::Math::Vector invertNormal);

        bool sub_822024F0(SoX::Math::Vector& position, SoX::Math::Vector& impulse, be<uint32_t>* postureFlag);

        bool sub_82202300(SoX::Math::Vector& position, SoX::Math::Vector& impulse, be<uint32_t>* postureFlag);

        ICollisionListener* sub_82201058(SoX::Math::Vector position, SoX::Math::Vector endPosition, be<uint32_t>* flag);

        ICollisionListener* sub_822011E8(SoX::Math::Vector d1, SoX::Math::Vector d2, be<uint32_t>* outFlag1);

        ICollisionListener* sub_82203840(SoX::Math::Vector* d1, SoX::Math::Vector d2, be<uint32_t>* outFlag1);

        ICollisionListener* sub_82201350(SoX::Math::Vector& pos1, SoX::Math::Vector& pos2, be<uint32_t>* outFlag1);
    };

    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_spPhysicsWorld, 0x10);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_spGroundListener, 0x14);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_spWallListener, 0x1C);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_spAllListener1, 0x24); //Sonicteam::Player::ICollisionListenerTemplate<Sonicteam::SoX::Physics::ShapeCastListener,Sonicteam::SoX::Physics::ShapeCastEvent>
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_spAllListener2, 0x2C); //Sonicteam::Player::ICollisionListenerTemplate<Sonicteam::SoX::Physics::ShapeCastListener,Sonicteam::SoX::Physics::ShapeCastEvent>
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_spGroundRayListener, 0x34);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_spRealGroundRayListener, 0x3C);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_spWallRayListener, 0x44);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_aShapeCollisionAll, 0x4C);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_aShapeCollisionWalk, 0x54);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_aShapeCollisionWall, 0x5C);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_RayCast, 0x64);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_pPostureRequestFlag, 0x68);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_CollisionAllAngle, 0x70);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_aCollisionAllRadius, 0x80);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_CollisionWalkAngle, 0x90);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_aCollisionWalkRadius, 0xA0);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_CollisionWallAngle, 0xB0);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_aCollisionWallRadius, 0xC0);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_FlagC8, 0xC8);
}

#include "IPostureSupportSphere.inl"
