#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/ICollisionListener.h>

namespace Sonicteam::Player
{
    template <typename TListener, typename TEvent>
    class ICollisionListenerTemplate : public ICollisionListener, public TListener {};
}
