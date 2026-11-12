#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Scenery
{
    class WorldIntersection : public RefCountObject {};

    MARATHON_ASSERT_SIZEOF(WorldIntersection, 0x08);
}
