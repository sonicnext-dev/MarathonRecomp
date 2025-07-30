#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Object.h>

namespace Sonicteam::SoX
{
    class Component : public Object
    {
    public:
        MARATHON_INSERT_PADDING(0x1C);
    };
}
