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

    auto pTailsFlight = pPlayer->m_spStateMachine->GetBase()->GetState<Sonicteam::Player::State::TailsFlight>();
    auto pGameImp = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::GameMode>()->m_pGameImp;

    auto maturityValue = 1.0f;

    // Set maturity value if the current state is Sonicteam::Player::State::TailsFlight.
    if (pTailsFlight->m_pVftable.ptr == 0x82005404)
        maturityValue = (1.0f / pTailsContext->m_FlightLimit) * pTailsFlight->m_FlightTime;

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

bool PlayerDebugMode_RemapDebugExitButton(PPCRegister& r_PadState, PPCRegister& r_out)
{
    auto pPadState = (Sonicteam::SoX::Input::PadState*)(g_memory.Translate(r_PadState.u32));

    return pPadState->IsPressed(Sonicteam::SoX::Input::KeyState_Select);
}

// Sonicteam::Player::Object::Update
PPC_FUNC_IMPL(__imp__sub_82195500);
PPC_FUNC(sub_82195500)
{
    if (!(Config::EnableDebugMode || Config::RestoreDemoCameraMode))
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
            auto padID = pDoc->m_PlayerControllerID[playerIndex];
            auto& spManager = pDoc->m_vspInputManager[padID];
            auto pGameImp = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::GameMode>()->m_pGameImp.get();

            // Disable collision on B press.
            if (pPlayer->m_SetupModuleIndexPostfix == 2 && spManager->m_PadState.IsPressed(Sonicteam::SoX::Input::KeyState_B) && Config::EnableDebugMode)
            {
                auto value = pZock->m_spPhantomA->m_pRigidBody->m_collidable.m_broadPhaseHandle.m_collisionFilterInfo == 6 ? 0x383 : 6;
                pZock->m_spPhantomA->m_pRigidBody->m_collidable.m_broadPhaseHandle.m_collisionFilterInfo = value;
                pGameImp->GetPhysicsWorld<Sonicteam::SoX::Physics::Havok::WorldHavok>()->m_pWorld->updateCollisionFilterOnWorld(1,1);
            }

            // Switch to Camera <-> DemoGMCamera
            if ((pPlayer->m_SetupModuleIndexPostfix == 2 || Config::RestoreDemoCameraMode) && spManager->m_PadState.IsPressed(Sonicteam::SoX::Input::KeyState_DpadUp))
            {
                auto pCamera = static_cast<Sonicteam::Camera::Cameraman*>(pPlayer->m_pCameraman.get());
                guest_stack_var<Sonicteam::Message::MsgCameramanCameraInitialize> m_initMessage;
                m_initMessage->IsDemoCam = pCamera->m_spCameraModeManager->m_spCameraMode->m_pVftable.ptr.get() != 0x82002004;
                m_initMessage->m_TargetActorID = pPlayer->m_ActorID;
                m_initMessage->m_PadID = spManager->m_ControllerID;
                pCamera->OnMessageReceived(m_initMessage.get());
            }

            // Enter debug posture on Select press.
            if (pPlayer->m_SetupModuleIndexPostfix != 2 &&
                !(pPlayer->m_SetupModuleIndexPrefix == 2 && pPlayer->m_SetupModuleIndexPostfix == 1) && Config::EnableDebugMode &&
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

bool RestoreChaosSpearFlips()
{
    return Config::RestoreChaosSpearFlips;
}

void RestoreChaosBoostJump(PPCRegister& r10, PPCRegister& r11)
{
    if (!Config::RestoreChaosBoostJump)
        return;

    r10.u32 = 1;
    r11.u32 = 2;
}

void RestoreChainJumpFlips(PPCRegister& r_ObjectPlayer, PPCRegister& r_Message, PPCRegister& r_ObjectContext, PPCRegister& f1, PPCRegister& f2, PPCRegister& f3)
{
    if (!Config::RestoreChainJumpFlips)
        return;

    struct Message0x11047
    {
        be<uint32_t> m_id;
        Sonicteam::SoX::Math::Quaternion m_rotation;
        Sonicteam::SoX::Math::Vector m_position;
        be<uint32_t> m_ActorID;
    };

    auto pObjectPlayer = static_cast<Sonicteam::Player::Object *>(g_memory.Translate(r_ObjectPlayer.u32));
    auto pMessage = static_cast<Message0x11047 *>(g_memory.Translate(r_Message.u32));
    auto pObjectContext = static_cast<Sonicteam::Player::State::ICommonContext *>(g_memory.Translate(r_ObjectContext.u32));
    auto speedHorizontal = f1.f64;
    auto speedForward = f2.f64;

    auto point = pObjectPlayer->m_spRootFrame->m_PositionF0;
    auto target = pMessage->m_position;

    if (pMessage->m_ActorID.get() != -1)
    {
        auto pFixture = GuestToHostFunction<Sonicteam::Fixture*>(sub_821609D0,App::s_pApp->m_pDoc->GetDocMode<Sonicteam::GameMode>()->m_pGameImp->m_spActorManager.get(), &pMessage->m_ActorID);
        auto sMessage = guest_stack_var<Sonicteam::Message::MsgObjJump123GetNextPoint>();
        sMessage->m_Rotation = { 0, 0, 0, 1 };
        sMessage->m_Position = { 0, 0, 0, 1 };

        if (pFixture->OnMessageReceived(sMessage))
        {
            target = sMessage->m_Position;
        }
    }

    auto distance = point.DistanceTo(target);
    double combinedSpeed = std::sqrt(speedHorizontal * speedHorizontal + speedForward * speedForward);
    double timeValue = 1.0; // Default fallback

    if (distance > 0.0 && combinedSpeed > 0.0)
    {
        timeValue = distance / combinedSpeed;
    }

    // CommonContext has a slightly different algorithm to process chain flips
    if (reinterpret_cast<Sonicteam::Player::IPlugIn*>(pObjectContext)->m_pVftable.get() != g_memory.Translate(0x8200A728))
    {
        timeValue *= 0.35;
    }

    f3.f64 = timeValue;
}

bool DisablePushState()
{
    return Config::DisablePushState;
}
