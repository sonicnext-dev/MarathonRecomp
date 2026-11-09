#pragma once

#include <hk330/hkWorldObject.h>

namespace hk330
{
    class hkEntity : public hkWorldObject
    {
    public:
        MARATHON_INSERT_PADDING(0x1E0);
    };
}
