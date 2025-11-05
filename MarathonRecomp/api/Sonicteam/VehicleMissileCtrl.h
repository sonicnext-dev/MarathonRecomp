#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyPhantom.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class MyAnimation;
    class MyModel;
    class VehicleLockOnListener;

    namespace Enemy
    {
        class ShotInfo;
        class ShotParameter;
    }

    namespace SoX::Graphics
    {
        class AnimationHierarchyCommon;
        class FrameGP;
        class InstanceModelCommon;
    }

    namespace SoX::Scenery
    {
        class Clump;
    }

    class VehicleMissileCtrl
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_MissileType;
        MARATHON_INSERT_PADDING(4);
        stdx::string m_ObjectName;
        stdx::string m_ShotName;
        be<float> m_MissileBoxX;
        be<float> m_MissileBoxY;
        be<float> m_MissileBoxZ;
        xpointer<Enemy::ShotInfo> m_pShotInfo;
        xpointer<Enemy::ShotParameter> m_pShotParameter;
        MARATHON_INSERT_PADDING(8);
        be<float> m_Field64;
        MARATHON_INSERT_PADDING(4);
        xpointer<MyPhantom> m_pMyPhantom;
        xpointer<VehicleLockOnListener> m_pVehicleLockOnListener;
        xpointer<SoX::Graphics::FrameGP> m_apRightGunFrameGPs[2];
        xpointer<MyModel> m_pRightGunModel;
        xpointer<SoX::Graphics::InstanceModelCommon> m_pRightGunInstanceModel;
        xpointer<SoX::Scenery::Clump> m_pRightGunClump;
        xpointer<MyAnimation> m_pRightGunMotion;
        xpointer<SoX::Graphics::AnimationHierarchyCommon> m_pRightGunMotionHierarchy;
        xpointer<SoX::Graphics::FrameGP> m_pLeftGunFrameGP;
        xpointer<MyModel> m_pLeftGunModel;
        xpointer<SoX::Graphics::InstanceModelCommon> m_pLeftGunInstanceModel;
        xpointer<SoX::Scenery::Clump> m_pLeftGunClump;
        xpointer<MyAnimation> m_pLeftGunMotion;
        xpointer<SoX::Graphics::AnimationHierarchyCommon> m_pLeftGunMotionHierarchy;
    };

    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_MissileType, 0x08);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_ObjectName, 0x10);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_ShotName, 0x2C);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_MissileBoxX, 0x48);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_MissileBoxY, 0x4C);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_MissileBoxZ, 0x50);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pShotInfo, 0x54);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pShotParameter, 0x58);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_Field64, 0x64);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pMyPhantom, 0x6C);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pVehicleLockOnListener, 0x70);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_apRightGunFrameGPs, 0x74);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pRightGunModel, 0x7C);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pRightGunInstanceModel, 0x80);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pRightGunClump, 0x84);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pRightGunMotion, 0x88);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pRightGunMotionHierarchy, 0x8C);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pLeftGunFrameGP, 0x90);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pLeftGunModel, 0x94);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pLeftGunInstanceModel, 0x98);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pLeftGunClump, 0x9C);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pLeftGunMotion, 0xA0);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrl, m_pLeftGunMotionHierarchy, 0xA4);
    MARATHON_ASSERT_SIZEOF(VehicleMissileCtrl, 0xA8);
}
