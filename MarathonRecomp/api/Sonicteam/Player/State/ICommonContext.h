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
        MARATHON_INSERT_PADDING(0x10);
        be<uint32_t> m_StateID;
        MARATHON_INSERT_PADDING(0x10);
        be<uint32_t> m_Buttons;
        MARATHON_INSERT_PADDING(0x18);
        be<int32_t> m_Field70;
        MARATHON_INSERT_PADDING(0x1C);
    };
}
