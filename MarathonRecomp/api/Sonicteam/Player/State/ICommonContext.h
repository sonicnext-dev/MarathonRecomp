#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/ContextSpeedAndJump.h>
#include <Sonicteam/Player/State/ICommonContextIF.h>
#include <Sonicteam/Player/State/IContext.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Player::State
{
    class ICommonContext : public IContext, public ICommonContextIF, public ContextSpeedAndJump
    {
    public:
        be<uint32_t> m_AnimationID;
        be<float> m_LockButtons;
        be<uint32_t> m_LastVelocityForward;
        be<uint32_t> m_LastVelocityVertical;
        be<float> m_LastLockButtons;
        be<uint32_t> m_Buttons;
		be<float> m_CurrentStickBorder;
        MARATHON_INSERT_PADDING(4);
        SoX::Math::Quaternion m_PadRotation;
		be<uint32_t> m_AnimationState;
		MARATHON_INSERT_PADDING(0x1C);

        uint32_t GetButtons()
        {
            return m_LockButtons.get() > 0.0 ? 0u : m_Buttons.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(ICommonContext, m_AnimationID, 0x40);
    MARATHON_ASSERT_OFFSETOF(ICommonContext, m_LockButtons, 0x44);
    MARATHON_ASSERT_OFFSETOF(ICommonContext, m_LastVelocityForward, 0x48);
    MARATHON_ASSERT_OFFSETOF(ICommonContext, m_LastVelocityVertical, 0x4C);
    MARATHON_ASSERT_OFFSETOF(ICommonContext, m_LastLockButtons, 0x50);
    MARATHON_ASSERT_OFFSETOF(ICommonContext, m_Buttons, 0x54);
    MARATHON_ASSERT_OFFSETOF(ICommonContext, m_CurrentStickBorder, 0x58);
    MARATHON_ASSERT_OFFSETOF(ICommonContext, m_AnimationState, 0x70);
    MARATHON_ASSERT_SIZEOF(ICommonContext, 0x90);
}
