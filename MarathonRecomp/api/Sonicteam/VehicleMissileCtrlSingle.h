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
}
