#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Object.h>

namespace Sonicteam::SoX::Input
{
    class Listener:SoX::Object
    {
    public:
        MARATHON_INSERT_PADDING(0x14); //LinkSoxNode<void> LinkInput, flags
    };
}
