#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ICollisionListener.h>

namespace Sonicteam::Player
{
    template <typename Cast, typename Event>
    class ICollisionListenerTemplate:ICollisionListener,Cast {};
}
