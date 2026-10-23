#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/IContext.h>
#include <Sonicteam/SoX/AI/State.h>

namespace Sonicteam::Player::State
{
    class Object2 : public SoX::AI::State<IContext> {};

    MARATHON_ASSERT_SIZEOF(Object2, 0x0C);
}
