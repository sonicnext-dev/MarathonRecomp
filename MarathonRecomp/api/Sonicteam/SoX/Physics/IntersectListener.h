#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Physics
{
    class IntersectListener : public SoX::RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(0x0C);
    };

    MARATHON_ASSERT_SIZEOF(IntersectListener, 0x14);
}
