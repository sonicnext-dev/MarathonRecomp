#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/ContextSpeedAndJump.h>
#include <Sonicteam/Player/State/ICommonContextIF.h>
#include <Sonicteam/Player/State/IContext.h>

namespace Sonicteam::Player::State
{
    class ICommonContext : public IContext, public ICommonContextIF, public ContextSpeedAndJump
    {
    public:
        MARATHON_INSERT_PADDING(0x60);
    };
}
