#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/Input/Listener.h>

namespace Sonicteam::Player::Input
{
    class IListener : public Listener
    {
    public:
        MARATHON_INSERT_PADDING(0x14);
    };

    MARATHON_ASSERT_SIZEOF(IListener, 0x18);
}
