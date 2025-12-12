#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/ShapeCastListener.h>

namespace Sonicteam::SoX::Physics
{
    class IntersectListener : public ShapeCastListener {};

    MARATHON_ASSERT_SIZEOF(IntersectListener, 0x8);
}
