#pragma once

#include <Physics/Shape.h>

namespace Phys
{
    class Entity:public Shape
    {
        MARATHON_INSERT_PADDING(0x5C);
    };
}

