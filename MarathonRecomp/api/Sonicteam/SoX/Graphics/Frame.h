#pragma once

#include <Marathon.inl>

#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Graphics
{
    class Frame : public SoX::RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(0x18); // SimpleNode
        MARATHON_INSERT_PADDING(0x2C);
    };
}
