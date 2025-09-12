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
        xpointer<SoX::Graphics::FrameGP> m_pRightGunFrameGPs[2];
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
}
