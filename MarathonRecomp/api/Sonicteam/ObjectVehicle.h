#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class ObjectVehicle : public Actor
    {
    public:
        MARATHON_INSERT_PADDING(0x180);
    };

    MARATHON_ASSERT_SIZEOF(ObjectVehicle, 0x1D8);
}
