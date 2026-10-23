#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::State
{
    class IMachine : public IPlugIn {};

    MARATHON_ASSERT_SIZEOF(IMachine, 0x20);
}
