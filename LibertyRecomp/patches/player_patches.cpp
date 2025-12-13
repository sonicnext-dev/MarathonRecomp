#include <api/Liberty.h>
#include <cpu/guest_stack_var.h>
#include <os/logger.h>
#include <user/config.h>
#include <app.h>

// Sonicteam::Player::Object::Update
PPC_FUNC_IMPL(__imp__sub_82195500);
PPC_FUNC(sub_82195500)
{
    auto pPlayer = (Sonicteam::Player::Object*)(base + ctx.r3.u32);
    auto pInputManager = pPlayer->GetInputManager();

    if (pPlayer->m_IsPlayer && pInputManager)
    {
        if (Config::EnableDebugMode)
        {
            switch (pPlayer->m_SetupModuleIndexPostfix)
            {
                case 1:
                {
                    // Toggle debug posture on Select press.
                    if (pPlayer->m_SetupModuleIndexPrefix == 1 && pInputManager->m_PadState.IsPressed(Sonicteam::SoX::Input::KeyState_Select))
                    {
                        pPlayer->m_SetupModuleIndexPostfix = 2;

                        LOGN("Debug Mode: Enabled");
                    }

                    break;
                }

                case 2:
                {
                    // Toggle camera volume collision on B press.
                    if (pInputManager->m_PadState.IsPressed(Sonicteam::SoX::Input::KeyState_B))
                    {
                        auto pGame = App::s_pApp->GetGame();
                        auto pZock = pPlayer->GetPlugin<Sonicteam::Player::Zock>("zock");
                        auto collisionFilterInfo = pZock->m_spPhantomA->m_pRigidBody->m_collidable.m_broadPhaseHandle.m_collisionFilterInfo == 6 ? 0x383 : 6;

                        LOGFN("Camera Volumes: {}", collisionFilterInfo != 6 ? "Enabled" : "Disabled");

                        pZock->m_spPhantomA->m_pRigidBody->m_collidable.m_broadPhaseHandle.m_collisionFilterInfo = collisionFilterInfo;
                        pGame->GetPhysicsWorld<Sonicteam::SoX::Physics::Havok::WorldHavok>()->m_pWorld->updateCollisionFilterOnWorld(1, 1);
                    }

                    break;
                }
            }
        }

        // Toggle demo camera on right stick press.
        if (Config::RestoreDemoCameraMode && pInputManager->m_PadState.IsPressed(Sonicteam::SoX::Input::KeyState_RightStick))
        {
            auto pCameraman = static_cast<Sonicteam::Camera::Cameraman*>(pPlayer->m_pCameraman.get());

            if (auto pCameraMode = pCameraman->m_spCameraModeManager->m_spCameraMode.get())
            {
                guest_stack_var<Sonicteam::Message::MsgCameramanChangeMode> msgCameramanChangeMode;
                msgCameramanChangeMode->PadID = pInputManager->m_PadID;
                msgCameramanChangeMode->TargetActorID = pPlayer->m_ActorID;
                msgCameramanChangeMode->IsDemoCamera = pCameraMode->m_pVftable.ptr != 0x82002004;

                LOGFN("Demo Camera: {}", msgCameramanChangeMode->IsDemoCamera ? "Enabled" : "Disabled");

                pCameraman->ProcessMessage(msgCameramanChangeMode.get());
            }
        }
    }

    __imp__sub_82195500(ctx, base);
}

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
    auto pPlayer = pTailsContext->m_spScore->m_pPlayer;

    if (auto pGauge = pPlayer->GetGauge<Sonicteam::Player::SonicGauge>())
    {
        pGauge->c_gauge_max = 100.0f;
        pGauge->m_Value = (100.0f / pTailsContext->m_FlightDuration) * pTailsContext->m_FlightTime;
    }

    auto pTailsFlight = pPlayer->m_spStateMachine->GetBase()->GetState<Sonicteam::Player::State::TailsFlight>();
    auto pGame = App::s_pApp->GetGame();

    auto maturityValue = 1.0f;

    // Set maturity value if the current state is Sonicteam::Player::State::TailsFlight.
    if (pTailsFlight->m_pVftable.ptr == 0x82005404)
        maturityValue = (1.0f / pTailsContext->m_FlightLimit) * pTailsFlight->m_FlightTime;

    for (int i = 0; i < 4; i++)
    {
        if (pGame->m_PlayerData[i].ActorID == pPlayer->m_ActorID.get())
            pGame->m_PlayerData[i].MaturityValue = maturityValue;
    }

    __imp__sub_8221A7D8(ctx, base);
}

// Sonicteam::Player::Score::Score
PPC_FUNC_IMPL(__imp__sub_821E8C48);
PPC_FUNC(sub_821E8C48)
{
    if (!Config::TailsGauge)
    {
        __imp__sub_821E8C48(ctx, base);
        return;
    }

    auto pPlayer = (Sonicteam::Player::Object*)(base + ctx.r4.u32);

    if (pPlayer->m_LuaFile == "player/tails.lua")
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

// Add missing SetupModuleDebug to table.
void PlayerDebugMode_RegisterLuaSetup(PPCRegister& str, PPCRegister& index)
{
    if (!Config::EnableDebugMode)
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

bool PlayerDebugMode_RemapDebugExitButton(PPCRegister& r30)
{
    auto pPadState = (Sonicteam::SoX::Input::PadState*)g_memory.Translate(r30.u32);

    if (pPadState->IsPressed(Sonicteam::SoX::Input::KeyState_Select))
    {
        LOGN("Debug Mode: Disabled");
        return true;
    }

    return false;
}

bool AntigravityRetainsMomentum()
{
    return Config::AntigravityRetainsMomentum;
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

bool ControllableTeleportDash()
{
    return Config::ControllableTeleportDash;
}

bool DisablePushState()
{
    return Config::DisablePushState;
}

bool MidairControlForMachSpeed()
{
    return Config::MidairControlForMachSpeed;
}

bool MidairControlForSnowboards()
{
    return Config::MidairControlForSnowboards;
}

void RestoreChaosBoostJump(PPCRegister& r10, PPCRegister& r11)
{
    if (!Config::RestoreChaosBoostJump)
        return;

    r10.u32 = 1;
    r11.u32 = 2;
}

void RestoreChainJumpFlips(PPCRegister& r31, PPCRegister& r30, PPCRegister& r11, PPCRegister& f1, PPCRegister& f2, PPCRegister& f3)
{
    if (!Config::RestoreChainJumpFlips)
        return;

    struct Message
    {
        be<uint32_t> ID;
        Sonicteam::SoX::Math::Quaternion Rotation;
        Sonicteam::SoX::Math::Vector Position;
        be<uint32_t> ActorID;
    };

    auto pPlayer = (Sonicteam::Player::Object*)g_memory.Translate(r31.u32);
    auto pMessage = (Message*)g_memory.Translate(r30.u32);
    auto pPlayerContext = (Sonicteam::Player::State::ICommonContext*)g_memory.Translate(r11.u32);
    auto pActorManager = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::GameMode>()->GetGame()->m_spActorManager.get();

    auto origin = pPlayer->m_spRootFrame->m_PositionF0;
    auto target = pMessage->Position;

    if (pMessage->ActorID != -1)
    {
        auto pFixture = GuestToHostFunction<Sonicteam::Fixture*>(sub_821609D0, pActorManager, &pMessage->ActorID);

        auto msgObjJump123GetNextPoint = guest_stack_var<Sonicteam::Message::MsgObjJump123GetNextPoint>();
        msgObjJump123GetNextPoint->Rotation = { 0, 0, 0, 1 };
        msgObjJump123GetNextPoint->Position = { 0, 0, 0, 1 };

        if (pFixture->ProcessMessage(msgObjJump123GetNextPoint.get()))
            target = msgObjJump123GetNextPoint->Position;
    }

    auto magnitudeHorz = f1.f64;
    auto magnitudeForward = f2.f64;
    auto distance = origin.DistanceTo(target);
    auto magnitude = std::sqrt(magnitudeHorz * magnitudeHorz + magnitudeForward * magnitudeForward);
    auto time = 1.0;

    if (distance > 0.0 && magnitude > 0.0)
        time = distance / magnitude;

    // CommonContext has a slightly different algorithm to process chain flips.
    if (((Sonicteam::Player::IPlugIn*)pPlayerContext)->m_pVftable.ptr != 0x8200A728)
        time *= 0.35;

    f3.f64 = time;
}

bool RestoreChaosSpearFlips()
{
    return Config::RestoreChaosSpearFlips;
}

bool UnlimitedAntigravity()
{
    if (Config::SlidingAttack == ESlidingAttack::B)
        return true;

    return Config::UnlimitedAntigravity;
}

PPC_FUNC_IMPL(__imp__sub_82217FC0);
PPC_FUNC(sub_82217FC0)
{
    if (!Config::RestoreSonicActionGauge)
    {
        __imp__sub_82217FC0(ctx, base);
        return;
    }

    auto pSonicContext = (Sonicteam::Player::State::SonicContext*)(base + ctx.r3.u32);
    auto pSonicGauge = pSonicContext->m_Gauge.get();

    using enum Sonicteam::Player::State::SonicContext::Gem;

    auto gemIndex = (Sonicteam::Player::State::SonicContext::Gem)ctx.r4.u32;

    switch (gemIndex)
    {
        case Gem_Yellow:
        {
            // Prevent Yellow Gem spam.
            if (pSonicContext->m_ThunderGuard)
                break;
        }

        case Gem_Blue:
        case Gem_Green:
        case Gem_Sky:
        case Gem_White:
        case Gem_Super:
        {
            auto spriteIndex = Sonicteam::Player::State::SonicContext::ms_GemSpriteConversionTable[gemIndex - 1] - 1;

            if (pSonicGauge->m_Value >= (&pSonicGauge->c_gauge_green)[spriteIndex].get())
            {
                ctx.r3.u64 = 1;
                return;
            }

            break;
        }

        case Gem_Red:
        case Gem_Purple:
        {
            if (pSonicContext->m_Field24A == 0)
            {
                ctx.r3.u64 = 1;
                return;
            }

            break;
        }
    }

    ctx.r3.u64 = 0;
}

PPC_FUNC_IMPL(__imp__sub_82218068);
PPC_FUNC(sub_82218068)
{
    if (!Config::RestoreSonicActionGauge)
    {
        __imp__sub_82217FC0(ctx, base);
        return;
    }

    auto pSonicContext = (Sonicteam::Player::State::SonicContext*)(base + ctx.r3.u32);
    auto pSonicGauge = pSonicContext->m_Gauge.get();
    auto deltaTime = ctx.f1.f64;

    using enum Sonicteam::Player::State::SonicContext::Gem;

    auto gemIndex = (Sonicteam::Player::State::SonicContext::Gem)ctx.r4.u32;
    auto spriteIndex = Sonicteam::Player::State::SonicContext::ms_GemSpriteConversionTable[gemIndex - 1] - 1;

    switch (gemIndex)
    {
        case Gem_Blue:
        case Gem_Green:
        case Gem_Yellow:
        case Gem_Sky:
        case Gem_White:
        case Gem_Super:
        {
            pSonicGauge->m_Value = pSonicGauge->m_Value.get() - (&pSonicGauge->c_gauge_green)[spriteIndex].get();
            pSonicGauge->m_GroundedTime = 0.0;
            break;
        }

        case Gem_Red:
        case Gem_Purple:
        {
            pSonicGauge->m_Value = pSonicGauge->m_Value.get() - (&pSonicGauge->c_gauge_green)[spriteIndex].get() * deltaTime;
            pSonicGauge->m_GroundedTime = 0.0;

            if (pSonicGauge->m_Value <= 0)
            {
                pSonicGauge->m_Value = 0.0;
                pSonicContext->m_Shrink = 0;
                pSonicContext->m_SlowTime = 0;
                pSonicContext->m_Field24A = 1;
            }

            break;
        }
    }
}

// Sonicteam::Player::SonicGauge::IVariable::Init
// This hook redirects the incorrectly named Lua variables to the ones actually used in the scripts.
PPC_FUNC_IMPL(__imp__sub_8223F360);
PPC_FUNC(sub_8223F360)
{
    auto pIVariable = ctx.r3.u32;
    auto pLuaSystem = ctx.r4.u32;

    __imp__sub_8223F360(ctx, base);

    if (!Config::RestoreSonicActionGauge)
        return;

    auto pSonicGauge = (Sonicteam::Player::SonicGauge*)g_memory.Translate(pIVariable - 0x20);
    auto pVariableName = g_userHeap.Alloc<stdx::string>();

    *pVariableName = "c_gauge_green";

    if (pSonicGauge->c_gauge_green == 0.0f)
        pSonicGauge->c_gauge_green = GuestToHostFunction<float>(sub_821EA350, pLuaSystem, pVariableName);

    *pVariableName = "c_gauge_red";

    if (pSonicGauge->c_gauge_red == 0.0f)
        pSonicGauge->c_gauge_red = GuestToHostFunction<float>(sub_821EA350, pLuaSystem, pVariableName);

    *pVariableName = "c_gauge_blue";

    if (pSonicGauge->c_gauge_blue == 0.0f)
        pSonicGauge->c_gauge_blue = GuestToHostFunction<float>(sub_821EA350, pLuaSystem, pVariableName);

    *pVariableName = "c_gauge_white";

    if (pSonicGauge->c_gauge_white == 0.0f)
        pSonicGauge->c_gauge_white = GuestToHostFunction<float>(sub_821EA350, pLuaSystem, pVariableName);

    *pVariableName = "c_gauge_sky";

    if (pSonicGauge->c_gauge_sky == 0.0f)
        pSonicGauge->c_gauge_sky = GuestToHostFunction<float>(sub_821EA350, pLuaSystem, pVariableName);

    *pVariableName = "c_gauge_yellow";

    if (pSonicGauge->c_gauge_yellow == 0.0f)
        pSonicGauge->c_gauge_yellow = GuestToHostFunction<float>(sub_821EA350, pLuaSystem, pVariableName);

    *pVariableName = "c_gauge_purple";

    if (pSonicGauge->c_gauge_purple == 0.0f)
        pSonicGauge->c_gauge_purple = GuestToHostFunction<float>(sub_821EA350, pLuaSystem, pVariableName);

    *pVariableName = "c_gauge_super";

    if (pSonicGauge->c_gauge_super == 0.0f)
        pSonicGauge->c_gauge_super = GuestToHostFunction<float>(sub_821EA350, pLuaSystem, pVariableName);

    pVariableName->~string();

    g_userHeap.Free(pVariableName);
}
 
void SonicGauge_FixGemSprite(PPCRegister& r)
{
    auto pGame = (Sonicteam::GameImp*)g_memory.Translate(r.u32);

    for (int i = 0; i < 4; i++)
        pGame->m_PlayerData[i].GemIndex = 0;
}

void SonicGauge_FixFlags(PPCRegister& r3, PPCRegister& r31)
{
    if (!Config::RestoreSonicActionGauge || !r3.u32)
        return;

    auto pSonicGauge = (Sonicteam::Player::SonicGauge*)g_memory.Translate(r3.u32);

    // Ensure this is SonicGauge.
    if (((Sonicteam::Player::IPlugIn*)pSonicGauge)->m_pVftable.ptr != 0x8200D4D8)
        return;

    auto pSonicContext = (Sonicteam::Player::State::SonicContext*)g_memory.Translate(r31.u32);
    auto pSonicWeapons = pSonicContext->m_spScore->m_pPlayer->GetPlugin<Sonicteam::Player::Weapon::SonicWeapons>("sonic_weapons");

    static constexpr uint8_t s_groundedAnims[4] = { 0xCB, 0xCC, 0x46, 0xCE };

    for (auto i = 0; i < 4; i++)
    {
        if (pSonicContext->m_AnimationID == s_groundedAnims[i])
            pSonicGauge->m_GroundedFlags = 1;
    }

    if (pSonicContext->m_Tornado != 0 || pSonicWeapons->m_GunDrive.m_pElement)
    {
        pSonicGauge->m_GroundedFlags = 1;
    }
    else
    {
        using enum Sonicteam::Player::State::SonicContext::Gem;

        if ((pSonicContext->m_Buttons.get() & 0x10000) != 0)
            pSonicContext->m_Field24A = 0;

        if ((pSonicContext->m_Buttons.get() & 0x20000) != 0 && (pSonicContext->m_CurrentGem == Gem_Red || pSonicContext->m_CurrentGem == Gem_Purple))
        {
            pSonicGauge->m_GroundedFlags = 1;

            if (pSonicContext->m_Field24A)
            {
                pSonicGauge->m_GroundedFlags = 0;
                pSonicContext->m_Shrink = 0;
                pSonicContext->m_SlowTime = 0;
            }
        }
        else if ((pSonicContext->m_PostureFlags.get() & Sonicteam::Player::PostureControl::PostureFlag_Grounded) != 0 || pSonicContext->m_Field24A)
        {
            pSonicGauge->m_GroundedFlags = 0;
        }
    }
}

bool InfiniteLives()
{
    return Config::InfiniteLives;
}
