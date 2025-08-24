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

// SonicTeam::Player::Score::Score
PPC_FUNC_IMPL(__imp__sub_821E8C48);
PPC_FUNC(sub_821E8C48)
{
    if (!Config::TailsGauge)
    {
        __imp__sub_821E8C48(ctx, base);
        return;
    }

    auto pPlayer = (Sonicteam::Player::Object*)(base + ctx.r4.u32);

    if (pPlayer->m_PlayerLua == "player/tails.lua")
    {
        auto pSonicGauge = GuestToHostFunction<Sonicteam::Player::SonicGauge*>(sub_8223F208, g_userHeap.Alloc(sizeof(Sonicteam::Player::SonicGauge)));

        guest_stack_var<boost::shared_ptr<Sonicteam::Player::IGauge>> spSonicGauge;

        // Make shared pointer.
        GuestToHostFunction<void>(sub_821BEAB0, spSonicGauge.get(), pSonicGauge);

        // Add gauge plugin to player.
        GuestToHostFunction<void>(sub_821BECE0, pPlayer, spSonicGauge.get(), 1);

        pPlayer->m_spGauge = *spSonicGauge.get();
    }

    __imp__sub_821E8C48(ctx, base);
}

bool MidairMachSpeedControl()
{
    return Config::MidairControlForMachSpeed;
}

bool MidairSnowboardControl()
{
    return Config::MidairControlForSnowboards;
}

// Add missing SetupModuleDebug to table.
void DebugPlayerSwitch(PPCRegister& str, PPCRegister& index)
{
    if (!Config::DebugSwitch)
        return;

    auto pString = (stdx::string*)g_memory.Translate(str.u32);

    switch (index.u32)
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

bool DebugPlayerSwitchPostureSwitchButton(PPCRegister& r_PadState, PPCRegister& r_out)
{
    
    auto pPadState = (Sonicteam::SoX::Input::PadState*)(g_memory.Translate(r_PadState.u32));
    return pPadState->IsPressed(Sonicteam::SoX::Input::KeyState_Select);
}

PPC_FUNC_IMPL(__imp__sub_82195500);
PPC_FUNC(sub_82195500)
{
    if (!Config::DebugSwitch)
    {
        __imp__sub_82195500(ctx, base);
        return;
    }

    auto pPlayer = (Sonicteam::Player::Object*)(base + ctx.r3.u32);
    auto pDoc = App::s_pApp->m_pDoc;
    auto pZock = pPlayer->GetPlugin<Sonicteam::Player::Zock>("zock");
	
    if (auto pGameMode = pDoc->GetDocMode<Sonicteam::GameMode>())
    {

        if (pPlayer->m_IsPlayer)
        {
            auto playerIndex = pGameMode->m_pGameImp->PlayerActorIDToIndex(pPlayer->m_ActorID);
            auto& spManager = pDoc->m_vspInputManager[pDoc->m_PlayerControllerID[playerIndex]];

            if (pPlayer->m_SetupModuleIndexPostfix == 2 && spManager->m_PadState.IsPressed(Sonicteam::SoX::Input::KeyState_B))
            {
                auto pValue = (pZock->m_pPhantom->m_RigidBody->m_CollisionFilter = pZock->m_pPhantom->m_RigidBody->m_CollisionFilter.get() == 6) ? 0x383 : 6;
                if (pZock->m_pPhantom.get()) pZock->m_pPhantom->Release();
                if (pZock->m_pPhantomB.get()) pZock->m_pPhantomB->Release();
                GuestToHostFunction<void>(sub_821E3628, &pZock->m_pPhantom, &pPlayer->m_RootFrame,static_cast<Sonicteam::SoX::MessageReceiver*>(pPlayer), pValue, 50.0);
                pZock->m_pPhantomB = pZock->m_pPhantom;
                pZock->m_pPhantom->AddRef();
                GuestToHostFunction<void>(*(be<uint32_t>*)(g_memory.Translate(*(be<uint32_t>*)(pZock->m_pPhantom.get()) + (0x3C))), pZock->m_pPhantom.get(), &pGameMode->m_pGameImp->m_pPhysicsWorld);
                GuestToHostFunction<void>(*(be<uint32_t>*)(g_memory.Translate(*(be<uint32_t>*)(pZock->m_pPhantom.get()) + (0x58))), pZock->m_pPhantom.get(), (uint64_t)pZock + 0xA0);   
            }

            if (pPlayer->m_SetupModuleIndexPostfix != 2 && !(pPlayer->m_SetupModuleIndexPrefix == 2 && pPlayer->m_SetupModuleIndexPostfix == 1) && spManager->m_PadState.IsPressed(Sonicteam::SoX::Input::KeyState_Select))
                    pPlayer->m_SetupModuleIndexPostfix = 2;
        }
    }

    __imp__sub_82195500(ctx, base);
}
