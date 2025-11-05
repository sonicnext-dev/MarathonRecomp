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
        MARATHON_INSERT_PADDING(0xAC);
    };

    MARATHON_ASSERT_OFFSETOF(ObjectVehicleBike, m_pVehicleMissileCtrlAutomatic, 0x2B0);
    MARATHON_ASSERT_SIZEOF(ObjectVehicleBike, 0x360);
}
