#include <api/Marathon.h>
#include <cpu/guest_stack_var.h>
#include <os/logger.h>
#include <user/config.h>
#include <app.h>

// Sonicteam::Player::State::TailsContext::Update
PPC_FUNC_IMPL(__imp__sub_8221A7D8);
PPC_FUNC(sub_8221A7D8)
{
    if (!Config::TailsGauge)
    {
        __imp__sub_8221A7D8(ctx, base);
        return;
    }

    auto pTailsContext = (Sonicteam::Player::State::TailsContext*)(base + ctx.r3.u32);
    auto pPlayer = pTailsContext->m_pScore->m_pPlayer;

    if (auto pGauge = pPlayer->GetGauge<Sonicteam::Player::SonicGauge>())
    {
        pGauge->m_Maximum = 100.0f;
        pGauge->m_Value = (100.0f / pTailsContext->m_FlightDuration) * pTailsContext->m_FlightTime;
    }

    auto pState = pPlayer->m_spStateMachine->GetBase()->m_pState;
    auto pGameImp = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::GameMode>()->m_pGameImp;

    auto maturityValue = 1.0f;

    // Set maturity value if the current state is Sonicteam::Player::State::TailsFlight.
    if (pState->m_pVftable.ptr == 0x82005404)
        maturityValue = (1.0f / pTailsContext->m_FlightLimit) * pState->m_Time;

    for (int i = 0; i < 4; i++)
    {
        if (pGameImp->m_PlayerData[i].ActorID == pPlayer->m_ActorID.get())
            pGameImp->m_PlayerData[i].MaturityValue = maturityValue;
    }

    __imp__sub_8221A7D8(ctx, base);
}

// Sonicteam::Player::State::*Context IDynamicLink::Init
PPC_FUNC_IMPL(__imp__sub_8220F330);
PPC_FUNC(sub_8220F330)
{
    if (!Config::TailsGauge)
    {
        __imp__sub_8220F330(ctx, base);
        return;
    }

    auto pDynamicLink = (Sonicteam::Player::IDynamicLink*)(base + ctx.r3.u32);
    auto spPlugin = (boost::shared_ptr<Sonicteam::Player::IPlugIn>*)(base + ctx.r4.u32);

    // If this call is from TailsContext and this plugin is Sonicteam::Player::Score, set up the action gauge.
    // This is typically set up by OpenGauge in Lua, but we can't do this here.
    if (pDynamicLink->m_pVftable.ptr == 0x8200B7F4 && spPlugin->get()->m_Name == "score")
    {
        auto pScore = (Sonicteam::Player::Score*)spPlugin->get();
        auto pSonicGauge = GuestToHostFunction<Sonicteam::Player::SonicGauge*>(sub_8223F208, g_userHeap.Alloc(sizeof(Sonicteam::Player::SonicGauge)));

        guest_stack_var<boost::shared_ptr<Sonicteam::Player::IGauge>> spSonicGauge;

        // Make shared pointer.
        GuestToHostFunction<void>(sub_821BEAB0, spSonicGauge.get(), pSonicGauge);

        // Add gauge plugin to player.
        GuestToHostFunction<void>(sub_821BECE0, pScore->m_pPlayer.get(), spSonicGauge.get(), 1);

        pScore->m_pPlayer->m_spGauge = *spSonicGauge.get();
    }

    __imp__sub_8220F330(ctx, base);
}

bool MidairMachSpeedControl1()
{
    return Config::MidairControlForMachSpeed;
}

bool MidairMachSpeedControl2()
{
    return Config::MidairControlForMachSpeed;
}

bool MidairSnowboardControl1()
{
    return Config::MidairControlForSnowboards;
}

bool MidairSnowboardControl2()
{
    return Config::MidairControlForSnowboards;
}

bool MidairSnowboardControl3()
{
    return Config::MidairControlForSnowboards;
}


//Add Missing SetupModuleDebug to table
void DebugPlayerSwitch_0(PPCRegister& r_sstring, PPCRegister& r_index)
{
    if (!Config::DebugSwitch)
    {
        return;
    }

    auto pString = (stdx::string*)(g_memory.Translate(r_sstring.u32));
    auto Index = r_index.u32;
    switch (Index)
    {
    case 0:
        *pString = "SetupModuleDebug";
        break;
    case 1:
        *pString = "SetupModule";
        break;
    case 2:
        *pString = "SetupModuleDebug";
        break;
    }
}

PPC_FUNC_IMPL(__imp__sub_82195500);
PPC_FUNC(sub_82195500)
{
    if (!Config::DebugSwitch)
    {
        __imp__sub_82195500(ctx, base);
        return;
    }

    auto pDoc = App::s_pApp->m_pDoc;
    auto pGameImp = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::GameMode>()->m_pGameImp;
    auto pPlayer = (Sonicteam::Player::Object*)(base + ctx.r3.u32);


    if (pPlayer->m_SetupModuleIndexPost != -2 && pPlayer->isPlayer)
    {
        auto PIndex = pGameImp->PlayerActorIDToIndex(pPlayer->m_ActorID);
        auto PManager = pDoc->m_vspInputManager[pDoc->m_PlayerControllerID[PIndex].get()].get();
        if ((PManager->m_Gamepad.wLastButtons.get() & Sonicteam::SoX::Input::XENON_GAMEPAD_BACK) != 0)
        {
            pPlayer->m_SetupModuleIndexPost = 2;
        }
    }

    __imp__sub_82195500(ctx, base);
}
