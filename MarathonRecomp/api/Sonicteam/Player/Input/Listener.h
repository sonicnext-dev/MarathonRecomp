#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Object.h>

namespace Sonicteam::Player::Input
{
    class Listener : public SoX::Object {};

    MARATHON_ASSERT_SIZEOF(Listener, 0x04);
}
