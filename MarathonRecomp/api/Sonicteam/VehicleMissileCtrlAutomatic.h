#pragma once

#include <Marathon.inl>
#include <Sonicteam/VehicleMissileCtrl.h>

namespace Sonicteam
{
    class VehicleMissileCtrlAutomatic : public VehicleMissileCtrl
    {
    public:
        be<float> m_MissileInterval;
        be<uint32_t> m_MissileBullet;
        be<float> m_MissileRecoveryTime;
        be<float> m_MissileFrame;
        MARATHON_INSERT_PADDING(0x14);
        stdx::string m_VehicleName;
        MARATHON_INSERT_PADDING(8);
        bool m_IsShot;
        be<uint32_t> m_FieldF4;
        MARATHON_INSERT_PADDING(8);
    };

    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrlAutomatic, m_MissileInterval, 0xA8);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrlAutomatic, m_MissileBullet, 0xAC);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrlAutomatic, m_MissileRecoveryTime, 0xB0);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrlAutomatic, m_MissileFrame, 0xB4);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrlAutomatic, m_VehicleName, 0xCC);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrlAutomatic, m_IsShot, 0xF0);
    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrlAutomatic, m_FieldF4, 0xF4);
    MARATHON_ASSERT_SIZEOF(VehicleMissileCtrlAutomatic, 0x100);
}
