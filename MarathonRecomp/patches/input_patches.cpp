#include <api/Marathon.h>
#include <user/config.h>

// Sonicteam::Player::Input::ListenerNormal::Update
PPC_FUNC_IMPL(__imp__sub_82222428);
PPC_FUNC(sub_82222428)
{
    const auto pListenerNormal = reinterpret_cast<Sonicteam::Player::Input::ListenerNormal*>(base + ctx.r3.u32);
    const auto pInputManager = reinterpret_cast<Sonicteam::SoX::Input::Manager*>(base + ctx.r4.u32);

    __imp__sub_82222428(ctx, base);

    if (!*pListenerNormal->m_pIsListening)
        return;

    // TODO: revisit this for action remapping.
    if (Config::SlidingAttack == ESlidingAttack::B)
    {
        using ListenerState = Sonicteam::Player::Input::ListenerNormal::ListenerState;

        // Replace sliding attack mapping with B button.
        pListenerNormal->m_WestAccumulativeAction.m_Button = Sonicteam::SoX::Input::PadButton_East;

        // Allow exiting out of the sliding attack with the B button.
        if (pInputManager->PadState.IsReleased(Sonicteam::SoX::Input::PadButton(pListenerNormal->m_WestAccumulativeAction.m_Button.get())))
            pListenerNormal->m_State = pListenerNormal->m_State.get() | ListenerState::ListenerState_Bit12 | ListenerState::ListenerState_WestPressed3;
    }
    else
    {
        // Restore sliding attack mapping to X button.
        pListenerNormal->m_WestAccumulativeAction.m_Button = Sonicteam::SoX::Input::PadButton_West;
    }
}

void RemapLightDash(PPCRegister& r3, PPCRegister& r11)
{
    r11.u64 = ((r3.u32 >> (Config::LightDash == ELightDash::X ? 8 : 15)) & 1) != 0;
}
