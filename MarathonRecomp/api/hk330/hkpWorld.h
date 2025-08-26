#pragma once

#include <Marathon.inl>
#include <hk330/hkReferencedObject.h>

namespace hk330
{
    class hkpWorld : public hkReferencedObject
    {
    public:
        MARATHON_INSERT_PADDING(0x308);
    };
}
