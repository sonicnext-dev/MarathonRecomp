#pragma once

#include <hk330/hkpWorldObject.h>

namespace hk330
{
    class hkpEntity : public hkpWorldObject
    {
    public:
        MARATHON_INSERT_PADDING(0x1E0);
    };
}
