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
        MARATHON_INSERT_PADDING(0x8);
        bool m_IsShot;
        be<uint32_t> m_FieldF4;
        MARATHON_INSERT_PADDING(8);
    };
}
