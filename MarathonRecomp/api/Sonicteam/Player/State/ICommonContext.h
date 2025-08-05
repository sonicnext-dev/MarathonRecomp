#pragma once
#include <Marathon.inl>
#include <Sonicteam/Player/State/ContextSpeedAndJump.h>
#include <Sonicteam/Player/State/ICommonContextIF.h>
#include <Sonicteam/Player/State/IContext.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Math/Quaternion.h>


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
		be<float> m_CurrentStickBorder; // 0 .. 1.0
        MARATHON_INSERT_PADDING(4);
		be<uint32_t> m_AnimationState; // & 2 == 0 (Animation End, no any idea about others)
		MARATHON_INSERT_PADDING(0x1C);
    };
	//0x90

}
