#include <api/Liberty.h>
#include <user/config.h>

static constexpr int INPUT_LISTENER_B_DOWN = 0x2000000;

// Sonicteam::Player::Input::ListenerNormal::Update
PPC_FUNC_IMPL(__imp__sub_82222428);
PPC_FUNC(sub_82222428)
{
    auto pListenerNormal = (Sonicteam::Player::Input::ListenerNormal*)(base + ctx.r3.u32);
    auto pInputManager = (Sonicteam::SoX::Input::Manager*)(base + ctx.r4.u32);

    __imp__sub_82222428(ctx, base);

    if (*pListenerNormal->m_pIsListening)
    {
        auto& rPadState = pInputManager->m_PadState;

        // Add B down support to the normal input listener.
        if (rPadState.IsDown(Sonicteam::SoX::Input::KeyState_B))
            pListenerNormal->m_State = pListenerNormal->m_State.get() | INPUT_LISTENER_B_DOWN;
    }
}

void RemapAntigravityEnter(PPCRegister& r11, PPCRegister& r28)
{
    auto state = 0x800;

    if (Config::SlidingAttack == ESlidingAttack::B)
        state = INPUT_LISTENER_B_DOWN;

    r11.u64 = (r28.u32 & state) != 0;
}

void RemapAntigravityExit(PPCRegister& r11, PPCRegister& r30)
{
    if (Config::SlidingAttack == ESlidingAttack::X)
        return;

    r11.u64 = (r30.u32 & INPUT_LISTENER_B_DOWN) == 0;
}

void RemapLightDash(PPCRegister& r3, PPCRegister& r11)
{
    r11.u64 = ((r3.u32 >> (Config::LightDash == ELightDash::X ? 8 : 15)) & 1) != 0;
}
