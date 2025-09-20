#pragma once

#include <Marathon.inl>
#include <Sonicteam/VehicleMissileCtrlAutomatic.h>

namespace Sonicteam
{
    class ObjectVehicleBike : public ObjectVehicle
    {
    public:
        MARATHON_INSERT_PADDING(0xD8);
        xpointer<VehicleMissileCtrlAutomatic> m_pVehicleMissileCtrlAutomatic;
    };
}
