#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class VehicleMissileCtrlSingle : public VehicleMissileCtrl
    {
    public:
        be<float> m_MissileRechargeTime;
        MARATHON_INSERT_PADDING(0x18);
    };

    MARATHON_ASSERT_OFFSETOF(VehicleMissileCtrlSingle, m_MissileRechargeTime, 0xA8);
    MARATHON_ASSERT_SIZEOF(VehicleMissileCtrlSingle, 0xC4);
}
