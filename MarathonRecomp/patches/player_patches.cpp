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
void PlayerDebugMode_RegisterLuaSetup(PPCRegister& str, PPCRegister& index)
{
    if (!Config::PlayerDebugMode)
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

bool PlayerDebugMode_RemapDebugExitButton(PPCRegister& r_PadState, PPCRegister& r_out)
{
    auto pPadState = (Sonicteam::SoX::Input::PadState*)(g_memory.Translate(r_PadState.u32));

    return pPadState->IsPressed(Sonicteam::SoX::Input::KeyState_Select);
}

// Sonicteam::Player::Object::Update
PPC_FUNC_IMPL(__imp__sub_82195500);
PPC_FUNC(sub_82195500)
{
    if (!Config::PlayerDebugMode)
    {
        __imp__sub_82195500(ctx, base);
        return;
    }

    auto pPlayer = (Sonicteam::Player::Object*)(base + ctx.r3.u32);
    auto pZock = pPlayer->GetPlugin<Sonicteam::Player::Zock>("zock");
    auto pDoc = App::s_pApp->m_pDoc;
	
    if (auto pGameMode = pDoc->GetDocMode<Sonicteam::GameMode>())
    {
        if (pPlayer->m_IsPlayer)
        {
            auto playerIndex = pGameMode->m_pGameImp->PlayerActorIDToIndex(pPlayer->m_ActorID);
            auto& spManager = pDoc->m_vspInputManager[pDoc->m_PlayerControllerID[playerIndex]];

            // Disable camera volume collision on B press.
            if (pPlayer->m_SetupModuleIndexPostfix == 2 && spManager->m_PadState.IsPressed(Sonicteam::SoX::Input::KeyState_B))
            {
                auto value = pZock->m_spPhantomA->m_pRigidBody->m_collidable.m_broadPhaseHandle.m_collisionFilterInfo == 6 ? 0x383 : 6;

                pZock->m_spPhantomA.reset();
                pZock->m_spPhantomB.reset();

                GuestToHostFunction<void>(sub_821E3628, &pZock->m_spPhantomA, &pPlayer->m_spRootFrame, static_cast<Sonicteam::SoX::MessageReceiver*>(pPlayer), value, 50.0);

                pZock->m_spPhantomB = pZock->m_spPhantomA;
                pZock->m_spPhantomA->InitializeToWorld(pGameMode->m_pGameImp->m_spPhysicsWorld);
                pZock->m_spPhantomA->SetPhantomListener(pZock->m_spPhantomListener); 
            }

            // Enter debug posture on Select press.
            if (pPlayer->m_SetupModuleIndexPostfix != 2 &&
                !(pPlayer->m_SetupModuleIndexPrefix == 2 && pPlayer->m_SetupModuleIndexPostfix == 1) &&
                spManager->m_PadState.IsPressed(Sonicteam::SoX::Input::KeyState_Select))
            {
                pPlayer->m_SetupModuleIndexPostfix = 2;
            }
        }
    }

    __imp__sub_82195500(ctx, base);
}

bool ControllableTeleportDash()
{
    return Config::ControllableTeleportDash;
}

bool ControllableBoundAttack()
{
    return Config::ControllableBoundAttack;
}

bool ControllableBoundAttack2(PPCCRRegister& cmp)
{
    if (Config::ControllableBoundAttack)
        return !cmp.eq;

    return cmp.eq;
}

bool ControllableSpinkick()
{
    return Config::ControllableSpinkick;
}


///////////////////////////////////////////////////////
/////////////   Sonic Gauge Restoration ////////////////
////////////////////////////////////////////////////////

const Sonicteam::Player::State::SonicContext::GemsS gemConversionTable[] = {
    Sonicteam::Player::State::SonicContext::GemsS::SBlue,   
    Sonicteam::Player::State::SonicContext::GemsS::SRed,    
    Sonicteam::Player::State::SonicContext::GemsS::SGreen,  
    Sonicteam::Player::State::SonicContext::GemsS::SPurple, 
    Sonicteam::Player::State::SonicContext::GemsS::SSky,    
    Sonicteam::Player::State::SonicContext::GemsS::SWhite,  
    Sonicteam::Player::State::SonicContext::GemsS::SYellow, 
    Sonicteam::Player::State::SonicContext::GemsS::SSuper   
};

//Check Gauge Drain
//SonicTeam::Player::SonicGauge (IVariable), IVariable::Init(REF_TYPE(SonicTeam::LuaSystem))
PPC_FUNC_IMPL(__imp__sub_82217FC0);
PPC_FUNC(sub_82217FC0) {

    if (!Config::SonicGauge)
    {
        __imp__sub_82217FC0(ctx, base);
        return;
    }

    auto context = (Sonicteam::Player::State::SonicContext*)g_memory.Translate(ctx.r3.u32);
    auto gauge = context->m_Gauge.get();

    using enum Sonicteam::Player::State::SonicContext::Gems;
    auto gem_id = (Sonicteam::Player::State::SonicContext::Gems)(ctx.r4.u32);

    switch (gem_id) 
    {
        case Yellow:
            if (context->m_ThunderGuard) break;
        case Blue:
        case Green:
        case Sky:
        case White:
        case Super: 
        {
            size_t index = gemConversionTable[gem_id - 1] - 1;
            if (gauge->m_Value >= (&gauge->m_Green)[index].get()) 
            {
                ctx.r3.u64 = 1;
                return;
            }
            break;
        }
        case Red:
        case Purple:
            if (context->m_24A == 0) 
            {
                ctx.r3.u64 = 1;
                return;
            }
            break;
    }
    ctx.r3.u64 = 0;
    return;

}
//Gauge Drain
PPC_FUNC_IMPL(__imp__sub_82218068);
PPC_FUNC(sub_82218068) {

    if (!Config::SonicGauge)
    {
        __imp__sub_82217FC0(ctx, base);
        return;
    }

    auto context = (Sonicteam::Player::State::SonicContext*)g_memory.Translate(ctx.r3.u32);
    auto gauge = context->m_Gauge.get();

    using enum Sonicteam::Player::State::SonicContext::Gems;
    auto gem_id = (Sonicteam::Player::State::SonicContext::Gems)(ctx.r4.u32);
    double delta = ctx.f1.f64;

    switch (gem_id) 
    {
    case Blue:
        case Green:
        case Yellow:
        case Sky:
        case White:
        case Super:
        {
            uint32_t index = gemConversionTable[gem_id - 1] - 1;
            gauge->m_Value = gauge->m_Value.get() - (&gauge->m_Green)[index].get();
            gauge->m_GroundedTime = 0.0;
            break;
        }
        case Red:
        case Purple:
        {
            uint32_t index = gemConversionTable[gem_id - 1] - 1;
            gauge->m_Value = gauge->m_Value.get() - (&gauge->m_Green)[index].get() * delta;
            gauge->m_GroundedTime = 0.0;
            if (gauge->m_Value <= 0)
            {
                gauge->m_Value = 0.0;
                context->m_Shrink = 0;
                context->m_SlowTime = 0;
                context->m_24A = 1;

            }
            break;
        }
    }
}


PPC_FUNC_IMPL(__imp__sub_8223F360);
PPC_FUNC(sub_8223F360) {

    auto IVariable = ctx.r3.u32;
    auto RefTypeLuaSystem = ctx.r4.u32;
    __imp__sub_8223F360(ctx, base);
    if (!Config::SonicGauge)
        return;

    auto gauge = (Sonicteam::Player::SonicGauge*)g_memory.Translate(IVariable - 0x20);
    auto buffer = g_userHeap.Alloc<stdx::string>();

    // if (gauge->m_Gem == 0.0) if m_Gem not intitialized by __imp__sub_8223F360
    *buffer = "c_gauge_green";
    if (gauge->m_Green.get() == 0.0)
        gauge->m_Green = GuestToHostFunction<float>(sub_821EA350, RefTypeLuaSystem, g_memory.MapVirtual(buffer));

    *buffer = "c_gauge_red";
    if (gauge->m_Red.get()== 0.0)
        gauge->m_Red = GuestToHostFunction<float>(sub_821EA350, RefTypeLuaSystem, g_memory.MapVirtual(buffer));

    *buffer = "c_gauge_blue";
    if (gauge->m_Blue.get() == 0.0)
        gauge->m_Blue = GuestToHostFunction<float>(sub_821EA350, RefTypeLuaSystem, g_memory.MapVirtual(buffer));

    *buffer = "c_gauge_white";
    if (gauge->m_White.get() == 0.0)
        gauge->m_White = GuestToHostFunction<float>(sub_821EA350, RefTypeLuaSystem, g_memory.MapVirtual(buffer));

    *buffer = "c_gauge_sky";
    if (gauge->m_Sky.get() == 0.0)
        gauge->m_Sky = GuestToHostFunction<float>(sub_821EA350, RefTypeLuaSystem, g_memory.MapVirtual(buffer));

    *buffer = "c_gauge_yellow";
    if (gauge->m_Yellow.get() == 0.0)
        gauge->m_Yellow = GuestToHostFunction<float>(sub_821EA350, RefTypeLuaSystem, g_memory.MapVirtual(buffer));

    *buffer = "c_gauge_purple";
    if (gauge->m_Purple.get() == 0.0)
        gauge->m_Purple = GuestToHostFunction<float>(sub_821EA350, RefTypeLuaSystem, g_memory.MapVirtual(buffer));

    *buffer = "c_gauge_super";
    if (gauge->m_Super.get() == 0.0)
        gauge->m_Super = GuestToHostFunction<float>(sub_821EA350, RefTypeLuaSystem, g_memory.MapVirtual(buffer));


    buffer->~string();
    g_userHeap.Free(buffer);

    /* For Testing
    gauge->m_Green = 10;
    gauge->m_Blue = 20;
    gauge->m_White = 40;
    gauge->m_Sky = 60;
    gauge->m_Yellow = 80;
    gauge->m_Super = 100;
    gauge->m_Red = 20;
    gauge->m_Purple = 40;
    */
}
 
void SonicGaugeRestorationGaugeGemSpriteResetFix(PPCRegister& r_GameImp) {

    Sonicteam::GameImp* pGameImp = (Sonicteam::GameImp*)g_memory.Translate(r_GameImp.u32);
    for (int i = 0; i < 4; i++)
        pGameImp->m_PlayerData[i].GemIndex = 0;
}

void SonicGaugeRestorationGaugeFlagFix(PPCRegister& r_gauge, PPCRegister& r_context) {

    if (!Config::SonicGauge || !r_gauge.u32)
        return;

    auto pGauge = (Sonicteam::Player::SonicGauge*)g_memory.Translate(r_gauge.u32);
    auto PContext = (Sonicteam::Player::State::SonicContext*)g_memory.Translate(r_context.u32);

    if ((uint32_t)(static_cast<Sonicteam::Player::IPlugIn*>(pGauge)->m_pVftable.get()) != 0x8200D4D8) // != SonicGauge 
        return;

    auto weapons = PContext->m_pScore->m_pPlayer->GetPlugin<Sonicteam::Player::Weapon::SonicWeapons>("sonic_weapons");
    if (PContext->m_Tornado != 0 || PContext->m_AnimationID == 0xCB || PContext->m_AnimationID == 0xCC || PContext->m_AnimationID == 0x46 || PContext->m_AnimationID == 0xCE ||  weapons->m_GunDrive.Entity != 0)
    {
        pGauge->m_GroundedFlags = 1; // Lock game
    }
    else
    {
        using enum Sonicteam::Player::State::SonicContext::Gems;
        if ((PContext->m_Buttons.get() & 0x10000) != 0) {
            PContext->m_24A = 0;
        }

        if ((PContext->m_Buttons.get() & 0x20000) != 0 && (PContext->m_CurrentGem == Red || PContext->m_CurrentGem == Purple))
        {
            pGauge->m_GroundedFlags = 1; 
            if (PContext->m_24A) 
            {
                pGauge->m_GroundedFlags = 0;
                PContext->m_Shrink = 0;
                PContext->m_SlowTime = 0;
            }
        }
        else if ((PContext->m_PostureFlag.get() & Sonicteam::Player::PostureControl::ePostureFlag_Ground) != 0 || PContext->m_24A)
        {
            pGauge->m_GroundedFlags = 0;

        }
    }

}
//Sonic Gauge Restoration
