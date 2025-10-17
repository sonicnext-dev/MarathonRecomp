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
        be<uint32_t> m_AnimationID;
        be<float> m_LockButtons;
        be<uint32_t> m_LastVelocityForward;
        be<uint32_t> m_LastVelocityVertical;
        be<uint32_t> m_LastLockButtons;
        be<uint32_t> m_Buttons;
		be<float> m_CurrentStickBorder;
        MARATHON_INSERT_PADDING(4);
		be<uint32_t> m_AnimationState;
		MARATHON_INSERT_PADDING(0x2C);
    };
}
