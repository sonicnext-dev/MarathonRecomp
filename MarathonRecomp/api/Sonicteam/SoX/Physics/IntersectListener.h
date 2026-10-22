#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/ShapeCastListener.h>

namespace Sonicteam::SoX::Physics
{
    class IntersectListener : public ShapeCastListener
    {
    public:
        MARATHON_INSERT_PADDING(0x0C);
    };

    MARATHON_ASSERT_SIZEOF(IntersectListener, 0x14);
}
