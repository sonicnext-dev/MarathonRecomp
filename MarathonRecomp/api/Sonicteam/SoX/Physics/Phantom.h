#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/Entity.h>

namespace Sonicteam::SoX::Physics
{
    class Phantom : public Entity {};

    MARATHON_ASSERT_SIZEOF(Phantom, 0x28);
}
