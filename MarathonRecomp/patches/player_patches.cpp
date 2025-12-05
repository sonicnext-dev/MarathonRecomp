#include <api/Marathon.h>
#include <cpu/guest_stack_var.h>
#include <os/logger.h>
#include <user/config.h>
#include <app.h>
#include <gpu/video.h>
#include <magic_enum/magic_enum.hpp>

static float EdgeSmothing = 12.0;
static float Gonst = 0.7f;

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
                if (pInputManager->m_PadState.IsDown(Sonicteam::SoX::Input::KeyState_DpadUp) || pInputManager->m_PadState.IsDown(Sonicteam::SoX::Input::KeyState_DpadDown))
                {

                    auto pModel = pPlayer->GetPlugin<Sonicteam::Player::ModelSonic>("model");
                    auto pLoad = pPlayer->GetPlugin<Sonicteam::Player::Load>("load");

   
                    pModel->m_spPackageShoesModel->m_spHeadClump->Reset();
                    pModel->m_spPackageShoesModel->m_spRootClump->Reset();
                    pModel->m_spPackageModel->m_spHeadClump->Reset();
                    pModel->m_spPackageModel->m_spRootClump->Reset();

                    pLoad->AddClump(pModel->m_spPackageShoesModel->m_spHeadClump.get());
                    pLoad->AddClump(pModel->m_spPackageShoesModel->m_spRootClump.get());
              
                  //  MakeRed(model->m_ESPShoes.ESPRoot.pESPS.get());
                    //MakeRed(model->m_ESPShoes.ESPRoot.pESPL);
                   // MakeRed(model->m_ESPShoes.ESPRoot.pESPE);
                    
                   // model->m_pPackageModelObject->m_spRootClump->Reset();
                   // model->m_pPackageModelObject->m_spHeadClump->Reset();

                   // model->m_spRootClump->Reset();
                   // 
                   // model->m_spRootShadowClump->Reset();
                   // model->m_spMouthClump->Reset();

                    //model->m_spHeadClump->Reset();
                  // model->m_spHeadShadowClump->Reset();
              

                    /*
                    auto manager = Sonicteam::SoX::ResourceManager::GetInstance();
                    auto modelMGR = Sonicteam::SoX::Graphics::ModelMgr::GetInstance();
                    if (manager->m_mResources.find(modelMGR->m_MgrIndex) != manager->m_mResources.end() && modelMGR->m_IsInResourceManager)
                    {
                        auto& resources = manager->m_mResources[modelMGR->m_MgrIndex];
                        printf("[\n");

                        for (auto it = resources.begin(); it != resources.end(); ++it)
                        {
                            if ((*it).first != "player/sonic_new/sonic_Root.xno")
                                continue;

                            auto& model = *it;
                            auto RTTI = stdx::VftableToRTTI(model.second->m_pVftable.get());
                            auto pModel = static_cast<Sonicteam::MyModel*>(model.second.get());
                            auto pStrategy = pModel->m_MyModelStrategyNN.get();

                            printf("  {\n");
                            printf("    \"name\": \"%s\",\n", model.first.c_str());
                            printf("    \"type\": \"%s\",\n", RTTI->typeDesc->name().c_str());
                            printf("    \"vftable\": \"%p\",\n", model.second->m_pVftable.get());
                            printf("    \"pointer\": \"%p\",\n", model.second.get());
                            for (auto& bone : pStrategy->m_aBoneTransform)
                            {
                                printf("    bone : %s\n", bone.Name.c_str());
                            }
                            for (auto& material : pStrategy->m_NNModel.vspMaterials)
                            {
                                auto pMaterial = material.get();

                                printf("    material-class-name %s\n", stdx::ClassToName(pMaterial).c_str());
                                printf("    material %p\n", pMaterial);
                                printf("        material sampler 1  %p\n", pMaterial->m_aspSampler[0].get());
                                printf("        material sampler 2  %p\n", pMaterial->m_aspSampler[1].get());
                                printf("        material sampler 3  %p\n", pMaterial->m_aspSampler[2].get());
                                printf("        material sampler 4  %p\n", pMaterial->m_aspSampler[3].get());
                            }

                            for (auto& material : pStrategy->m_NNModel.vspSamplers)
                            {
                                auto pMaterial = material.get();

                                printf("    sampler %p\n", pMaterial);
                            }
                            auto pVertBuffer = pStrategy->m_NNModel.pVertexBufferXenon.get();

                            Sonicteam::SoX::Graphics::DefaultVertex* pVertex;
                            pVertBuffer->Lock(pVertex);

                 
                            // Cast to D3DVERTEXELEMENT9 array (12 bytes per element)
                            GuestVertexDeclaration* pOriginalDecl = (GuestVertexDeclaration*)pVertBuffer->m_pVertexDeclaration.get();;

                            srand(static_cast<unsigned int>(time(NULL)));  // Seed with current time
                            for (int i = 0; i < pOriginalDecl->vertexElementCount; i++) 
                            {
                                auto pDecl = &pOriginalDecl->vertexElements[i];


                                auto type = magic_enum::enum_name((_D3DDECLTYPE)((int32_t)pDecl->type.get()));
                                auto usage = magic_enum::enum_name((_D3DDECLUSAGE)pDecl->usage);

                                printf("pOriginalDecl->vertexElements[%d].offset %x\n", i, pDecl->offset.get());
                                printf("pOriginalDecl->vertexElements[%d].type %s - %x\n", i,type.data(),pDecl->type.get());
                                printf("pOriginalDecl->vertexElements[%d].usage %s\n", i, usage.data());
                           
                                printf("----\n");
                         
                            }

                            auto ppVertex = &pVertex[0];
                            for (int i = 0; i < pVertBuffer->m_VertexCount; i++) 
                            {
                                auto& color = ppVertex->Get<be<uint32_t>>(D3DDECLUSAGE_COLOR, (D3DVERTEXELEMENT9*)pOriginalDecl->vertexElements.get(), pOriginalDecl->vertexElementCount);
                                color = D3DCOLOR_ARGB(1, 255, 255, 255);
                                ppVertex = ppVertex->GetNext<Sonicteam::SoX::Graphics::DefaultVertex>((D3DVERTEXELEMENT9*)pOriginalDecl->vertexElements.get(), pOriginalDecl->vertexElementCount);
                            }

                            printf("pVertex %p\n", pVertex);


                            pVertBuffer->Unlock();
                        

                            printf("    \"m_MyModelStrategyNN\": \"%p\",\n", pModel->m_MyModelStrategyNN.get());
                            printf("    \"pVertexBufferXenon\": \"%p\",\n", pStrategy->m_NNModel.pVertexBufferXenon.get());
                            printf("    \"pIndexBufferXenon\": \"%p\",\n", pStrategy->m_NNModel.pIndexBufferXenon.get());
                            printf("  }\n");
                        }
                    }
                    */
                }

                // Toggle debug posture on Select press.
                if (pPlayer->m_SetupModuleIndexPrefix == 1 && pInputManager->m_PadState.IsPressed(Sonicteam::SoX::Input::KeyState_Select))
                {
                    pPlayer->m_SetupModuleIndexPostfix = 2;

                    LOGN("Debug Mode: Enabled");
                }
                if (pInputManager->m_PadState.IsDown(Sonicteam::SoX::Input::KeyState_DpadUp))
                {
                    EdgeSmothing += 1.0 * ctx.f1.f64;
                    printf("EdgeSmothing %f\n", EdgeSmothing);
                }

                if (pInputManager->m_PadState.IsDown(Sonicteam::SoX::Input::KeyState_DpadDown))
                {
                    EdgeSmothing -= 1.0 * ctx.f1.f64;
                    printf("EdgeSmothing %f\n", EdgeSmothing);
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

    auto pTailsFlight = pPlayer->GetStateMachine()->GetState<Sonicteam::Player::State::TailsFlight>();

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

bool ObjectPsiCommon_DisableHeightLimit()
{
    return Config::DisableSilverPsychoRideHeightLimit;
}

#if MARATHON_POSTURECONTROL_USE_RECREATED_VERSION == 1
PPC_FUNC_IMPL(__imp__sub_82200538);
PPC_FUNC(sub_82200538)
{
    auto PostureControl = (Sonicteam::Player::PostureControl*)(base + ctx.r3.u32);
    PostureControl->RecreatedUpdate(ctx.f1.f64);
}
#endif

//Experimental Edge OC Fix
PPC_FUNC_IMPL(__imp__sub_82203840);
PPC_FUNC(sub_82203840)
{
    auto pSphere = (Sonicteam::Player::IPostureSupportSphere*)(base + ctx.r3.u32);
    auto d1 = (Sonicteam::SoX::Math::Vector*)(base + ctx.r4.u32);
    auto d2 = *(Sonicteam::SoX::Math::Vector*)(base + ctx.r5.u32);
    auto outFlag1 = (be<uint32_t>*)(base + ctx.r6.u32);

    if (!Config::EnablePostureImprovements)
    {
        __imp__sub_82203840(ctx, base);
        return;
    }

    Sonicteam::SoX::Math::Vector finalPos = d2;

    if (auto allListener = pSphere->m_spAllListener1.get())
    {
        auto PNormal = allListener->m_aContactNormals[0];
        auto sphereCenter = d2 + pSphere->m_CollisionAllAngle;
        struct _vector_ { Sonicteam::SoX::Math::Vector p1; Sonicteam::SoX::Math::Vector p2; };
        guest_heap_var<_vector_> points(sphereCenter, sphereCenter);

        allListener->m_Position = sphereCenter;
        allListener->m_Radius = pSphere->m_aCollisionAllRadius[pSphere->m_FlagC8];
        allListener->SetPostureRequestFlag(pSphere->m_pPostureRequestFlag.get()->get());

        pSphere->m_aShapeCollisionAll[pSphere->m_FlagC8]->Func3C(
            (Sonicteam::SoX::Math::Vector*)points.get(),
            static_cast<Sonicteam::SoX::Physics::ShapeCastListener*>(allListener)
        );
        allListener->Update();

        if ((uint8_t*)outFlag1 != g_memory.base) *outFlag1 = allListener->GetCollisionFlag();

        auto contactPos = allListener->GetContactPosition() - pSphere->m_CollisionAllAngle;
        int contactCount = allListener->GetCurrentSurfaceContacts();

        if (contactCount > 0)
        {
            // PROPER SURFACE ANALYSIS
            int floorContacts = 0;
            int wallContacts = 0;
            Sonicteam::SoX::Math::Vector primaryNormal = allListener->m_aContactNormals[0];

            for (int i = 0; i < contactCount && i < 6; i++)
            {
                auto& normal = allListener->m_aContactNormals[i];
                printf("normal Y : %f\n\n", normal.Y.get());
                if (normal.Y > 0.39f) // Floor (pointing up)
                {
                    floorContacts++;
                }
                else if (normal.Y < -0.5f) // Ceiling (pointing down)  
                {
                    // Ceiling contact
                }
                else // Wall (vertical)
                {
                    wallContacts++;
                }
            }

            printf("SURFACE: %d floors, %d walls, primaryNormal=(%.2f,%.2f,%.2f)\n",
                floorContacts, wallContacts, primaryNormal.X.get(), primaryNormal.Y.get(), primaryNormal.Z.get());

            finalPos = contactPos;

            if (floorContacts >= 2 && wallContacts == 0)
            {
                bool isTrueEdge = false;
                for (int i = 0; i < contactCount - 1 && !isTrueEdge; i++)
                {
                    for (int j = i + 1; j < contactCount && !isTrueEdge; j++)
                    {
                        float dot = allListener->m_aContactNormals[i].Dot(allListener->m_aContactNormals[j]).X;
                        if (dot < 0.7f) // Significantly different normals
                        {
                            isTrueEdge = true;
                            printf("TRUE EDGE: Applying minimal stabilization (dot=%.3f)\n", dot);
                        }
                        else if (dot < 0.98)
                        {
                            isTrueEdge = true;
                            printf("ALT TRUE EDGE: Applying minimal stabilization (dot=%.3f)\n", dot);
                        }
                        
                    }
                }

                if (isTrueEdge)
                {
                    // Use the highest Y normal for stabilization
                    Sonicteam::SoX::Math::Vector bestNormal = primaryNormal;
                    for (int i = 1; i < contactCount && i < 6; i++)
                    {
                        if (allListener->m_aContactNormals[i].Y > bestNormal.Y)
                        {
                            bestNormal = allListener->m_aContactNormals[i];
                        }
                    }

                    // TINY push in best normal direction
                    finalPos = d2;
                }
                else if (floorContacts == 2)
                {
                    // Two similar floor contacts - micro stabilization only
                    finalPos = finalPos + (primaryNormal * 2.0);
                    printf("DOUBLE FLOOR: Micro stabilization\n");
                }
            }
            else if (floorContacts > 0 && wallContacts > 0)
            {
                // Floor-wall mix - small stabilization
                finalPos.Y = finalPos.Y  + 0.02f;
                printf("FLOOR-WALL: Minimal stabilization\n");
            }
            else
            {
                // Single floor or other cases - use raw contact position
                printf("Using raw contact position\n");
            }
        }
        else
        {
            finalPos = d2;
        }
    }

    if (d1) *d1 = finalPos;

    if (pSphere->m_spAllListener1.get()->GetCurrentSurfaceContacts() > 0)
    {
        ctx.r3.u32 = (uint32_t)(uintptr_t)pSphere->m_spAllListener1.get();
    }
    else
    {
        ctx.r3.u32 = 0;
    }
}
    
/*
#define ToMem(Type, reg) \
    (reg.u32 == 0 ? (Type*)(0) : (Type*)(reg.u32 + base))

PPC_FUNC_IMPL(__imp__sub_821FF0F0);
PPC_FUNC(sub_821FF0F0)
{

    using V = Sonicteam::SoX::Math::Vector;
    using P = Sonicteam::Player::IPosturePlugIn;
    using Q = Sonicteam::SoX::Math::Quaternion;
    auto pThis = (Sonicteam::Player::PostureControl*)(ctx.r3.u32 + base);
    pThis->sub_821FF0F0(*ToMem(V, ctx.r4), ToMem(Q, ctx.r5), *ToMem(V, ctx.r6));
}
*/

//const Sonicteam::Player::State::CommonSpringTemplate<75, 16>
void ResetSpringTemplate(Sonicteam::Player::State::CommonSpringTemplate_Default* spring)
{
    //printf("ResetSpringTemplate %s\n", stdx::ClassToName(spring).c_str());
    if (!Config::AlwaysResetSpringState)
        return;

    spring->m_FieldC = 1.0;
    spring->m_Field10 = 0;
    spring->m_Field14 = 0;
    spring->m_Field18 = 0;
}

PPC_FUNC_IMPL(__imp__sub_82209CC8);
PPC_FUNC(sub_82209CC8)
{
    ResetSpringTemplate((Sonicteam::Player::State::CommonSpringTemplate_Default*)(base + ctx.r3.u32));
    __imp__sub_82209CC8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82209BE8);
PPC_FUNC(sub_82209BE8)
{
    ResetSpringTemplate((Sonicteam::Player::State::CommonSpringTemplate_Default*)(base + ctx.r3.u32));
    __imp__sub_82209BE8(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82209C58);
PPC_FUNC(sub_82209C58)
{
    ResetSpringTemplate((Sonicteam::Player::State::CommonSpringTemplate_Default*)(base + ctx.r3.u32));
    __imp__sub_82209C58(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8220E0F0);
PPC_FUNC(sub_8220E0F0)
{
    ResetSpringTemplate((Sonicteam::Player::State::CommonSpringTemplate_Default*)(base + ctx.r3.u32));
    __imp__sub_82209C58(ctx, base);
}

bool SonicSpindash_UpdateJump(PPCRegister& state)
{
    if (!Config::AllowSpinDashJump)
        return false;

    auto pState = (Sonicteam::Player::State::CommonObject*)(g_memory.base + state.u32);
    auto pContext = pState->m_pContext.get();

    if (pContext->GetButtons() & 1)
    {
        pState->m_pStateMachine->ChangeState(Sonicteam::Player::State::SonicState_Jump);
        return true;
    }
    return false;
}

bool SonicMachSpeed_UpdateNoMoreLock(PPCRegister& lock)
{
    if (!Config::DisableMachStateLockInput)
        return false;

    lock.f64 = 0.0;

    return true;
}

void SonicContext_UpdateAlwaysABlueGem(PPCRegister& context)
{
    using SonicContext = Sonicteam::Player::State::SonicContext;

    if (!(Config::AlwaysBlueGem | Config::EnableGemsAura))
        return;

    SonicContext* pSonicContext = reinterpret_cast<SonicContext*>(g_memory.base + context.u32);
    auto* pStateMachine = pSonicContext->m_spScore->m_pPlayer->GetStateMachine();
    auto pCurrentState = pStateMachine->GetState<Sonicteam::SoX::AI::State<Sonicteam::Player::State::IContext>>();

    if (!pCurrentState || !pSonicContext->m_GemsEnabled)
        return;

    // ----------------- Improved Gems Aura (no statics, continuity via copying) -----------------
    using SubAnimation = Sonicteam::SoX::Graphics::AnimationCore::SubAnimation;


    if (Config::EnableGemsAura)
    {
        auto pModel = pSonicContext->m_spScore->m_pPlayer->GetPlugin<Sonicteam::Player::ModelSonic>("model");
        auto pLoad = pSonicContext->m_spScore->m_pPlayer->GetPlugin<Sonicteam::Player::Load>("load");
        auto StateName = stdx::ClassToName(pCurrentState);

        auto IsYellowGem = pSonicContext->m_CurrentGem == SonicContext::Gem_Yellow && (pSonicContext->m_Buttons.get() & 0x10000) != 0 && GuestToHostFunction<bool>(sub_82217FC0, pSonicContext, SonicContext::Gem_Yellow);

        bool enableAura = pSonicContext->m_SlowTime || pSonicContext->m_Shrink || pSonicContext->m_MachAura ||
            pSonicContext->m_FPS || pSonicContext->m_ThrowGem ||
            StateName == "class Sonicteam::Player::State::SonicTornadoGround" ||
            StateName == "class Sonicteam::Player::State::SonicTornadoAir" ||
            StateName == "class Sonicteam::Player::State::SonicHomingSmash" ||
            StateName == "class Sonicteam::Player::State::SonicBlastJump" ||
            StateName == "class Sonicteam::Player::State::SonicGemSnipe" ||
            StateName == "class Sonicteam::Player::State::SonicGemThrow" ||
            StateName == "class Sonicteam::Player::State::SonicSuper" ||
            IsYellowGem;

        struct Color { be<float> r, g, b, a; };

        // Palettes
        const std::vector<std::array<float, 3>> RainbowPalette = {
            {1.0f, 0.639f, 0.017f}, {1.0f, 0.7f, 0.1f}, {1.0f, 0.8f, 0.2f}, {1.0f, 0.75f, 0.15f},
            {0.95f, 0.95f, 0.98f}, {1.0f, 1.0f, 1.0f}, {0.98f, 0.98f, 1.0f},
            {0.866f, 0.933f, 0.980f}, {0.588f, 0.627f, 0.710f}, {0.467f, 0.517f, 0.572f}
        };

        const std::vector<std::array<float, 3>> ThunderGuardPalette = {
            {1.0f, 1.0f, 0.2f}, {1.0f, 0.9f, 0.1f}, {0.8f, 0.8f, 0.0f}, {1.0f, 1.0f, 0.8f},
            {0.6f, 0.7f, 1.0f}, {0.3f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.9f, 0.95f, 1.0f}
        };

        // Create fade-out palette from base color
        auto CreateFadeOutPalette = [](float r, float g, float b, int steps = 8) {
            std::vector<std::array<float, 3>> palette;
            for (int i = 0; i < steps; ++i) {
                float t = (float)i / (steps - 1);
                float fade = 1.0f - t; // Fade from 1 to 0
                palette.push_back({ r * fade, g * fade, b * fade });
            }
            return palette;
            };

        // Fade-out palettes for each color
        const auto CyanFadePalette = CreateFadeOutPalette(0.0f, 0.8f, 1.0f);
        const auto GreenFadePalette = CreateFadeOutPalette(0.0f, 1.0f, 0.3f);
        const auto WhiteFadePalette = CreateFadeOutPalette(1.0f, 1.0f, 1.0f);
        const auto RedFadePalette = CreateFadeOutPalette(1.0f, 0.2f, 0.2f);
        const auto PurpleFadePalette = CreateFadeOutPalette(1.0f, 0.0f, 1.0f);
        const auto MagentaFadePalette = CreateFadeOutPalette(0.8f, 0.0f, 0.8f);
        const auto BlueFadePalette = CreateFadeOutPalette(0.2f, 0.4f, 1.0f);
        const auto ThunderGuardFadePalette = CreateFadeOutPalette(0.2f, 0.4f, 1.0f);

        // Helpers
        auto ApplyToAllSubAnims = [&](auto pMaterialAnim, auto applyFunc) {
            if (pMaterialAnim && pMaterialAnim->m_spMaterialAnimationObject) {
                for (auto& subAnimPair : pMaterialAnim->m_spMaterialAnimationObject->m_mspSubAnimations) {
                    applyFunc(subAnimPair.second.get());
                }
            }
            };

        auto CopyAllSubAnimations = [&](auto srcMaterialAnim, auto dstMaterialAnim) {
            if (srcMaterialAnim && dstMaterialAnim &&
                srcMaterialAnim->m_spMaterialAnimationObject && dstMaterialAnim->m_spMaterialAnimationObject) {

                auto& srcSubAnims = srcMaterialAnim->m_spMaterialAnimationObject->m_mspSubAnimations;
                auto& dstSubAnims = dstMaterialAnim->m_spMaterialAnimationObject->m_mspSubAnimations;

                for (auto& dstPair : dstSubAnims) {
                    auto srcIt = srcSubAnims.find(dstPair.first);
                    if (srcIt != srcSubAnims.end()) {
                        SubAnimation* srcAnim = srcIt->second.get();
                        SubAnimation* dstAnim = dstPair.second.get();
                        if (srcAnim && dstAnim && srcAnim->m_aAnimationData && dstAnim->m_aAnimationData) {
                            int copyCount = std::min(srcAnim->m_vKeyFramePoints.size(), dstAnim->m_vKeyFramePoints.size());
                            for (int i = 0; i < copyCount; ++i) {
                                ((Color*)dstAnim->m_aAnimationData.get())[i] = ((Color*)srcAnim->m_aAnimationData.get())[i];
                            }
                        }
                    }
                }
            }
            };

        auto PaintSolidAnim = [&](SubAnimation* anim, float r, float g, float b) {
            if (anim && anim->m_aAnimationData) {
                Color* c = (Color*)anim->m_aAnimationData.get();
                int count = anim->m_vKeyFramePoints.size();
                for (int i = 0; i < count; ++i) {
                    c[i].r = r;
                    c[i].g = g;
                    c[i].b = b;
                }
            }
            };

        auto PaintPaletteAnim = [&](SubAnimation* anim, const std::vector<std::array<float, 3>>& palette, float speed) {
            if (!anim || !anim->m_aAnimationData) return;

            Color* c = (Color*)anim->m_aAnimationData.get();
            int keyframeCount = anim->m_vKeyFramePoints.size();
            if (keyframeCount == 0) return;

            int paletteSize = (int)palette.size();

            for (int i = 0; i < keyframeCount; ++i) {
                float t = fmodf(anim->m_vKeyFramePoints[i] * speed, 1.0f);
                float segment = t * (paletteSize - 1);
                int idx0 = (int)floor(segment);
                int idx1 = (idx0 + 1) % paletteSize;
                float fraction = segment - idx0;

                c[i].r = palette[idx0][0] * (1 - fraction) + palette[idx1][0] * fraction;
                c[i].g = palette[idx0][1] * (1 - fraction) + palette[idx1][1] * fraction;
                c[i].b = palette[idx0][2] * (1 - fraction) + palette[idx1][2] * fraction;
            }
            };

        auto CreateHighResAnimation = [&](SubAnimation* anim, int targetKeyframes = 128) {
            if (!anim) return false;
            int currentKeyframes = anim->m_vKeyFramePoints.size();
            if (currentKeyframes >= targetKeyframes) return false;

            size_t newSize = sizeof(Color) * targetKeyframes;
            anim->m_aAnimationData = (xpointer<void>)g_userHeap.Alloc(newSize);
            if (!anim->m_aAnimationData) return false;

            anim->m_vKeyFramePoints.clear();
            for (int i = 0; i < targetKeyframes; ++i) {
                float progress = (float)i / (targetKeyframes - 1);
                float keyframePos = anim->m_FrameStart + (anim->m_FrameEnd - anim->m_FrameStart) * progress;
                anim->m_vKeyFramePoints.push_back(keyframePos);
            }
            return true;
            };

        // Process a single ESP part
        auto ProcessESPPart = [&](auto& part, const std::vector<std::array<float, 3>>& palette, float speed) {
           
            ApplyToAllSubAnims(part.pAnimationESPStart, [&](SubAnimation* anim) {
                CreateHighResAnimation(anim);
                PaintPaletteAnim(anim, palette, speed);
                });

            ApplyToAllSubAnims(part.pAnimationESPLoop, [&](SubAnimation* anim) {
                CreateHighResAnimation(anim);
                PaintPaletteAnim(anim, palette, speed);
                });
            };

        // Process both root and head
        auto ProcessBothParts = [&](auto processFunc) {
            processFunc(pModel->m_ESPShoes.ESPRoot);
            processFunc(pModel->m_ESPShoes.ESPHead);
            };

        if (enableAura)
        {
            // Handle colored states
            if (Config::EnableColoredAuraForGems)
            {
                // Yellow Gem - ThunderGuard (electric palette)
                if (IsYellowGem) {
                    ProcessBothParts([&](auto& part) {
                        if (part.ESPState != 2) {
                            ProcessESPPart(part, ThunderGuardPalette, 8.0f);
                        }
                        });
                }
                // Super Sonic - faster rainbow
                else if (StateName == "class Sonicteam::Player::State::SonicSuper") {
                    ProcessBothParts([&](auto& part) {
                        if (part.ESPState != 2) {
                            ProcessESPPart(part, RainbowPalette, part.ESPState == 0 ? 8.0f : 12.0f);
                        }
                        });
                }
                // Cyan states
                else if (pSonicContext->m_FPS || pSonicContext->m_ThrowGem ||
                    StateName == "class Sonicteam::Player::State::SonicGemSnipe" ||
                    StateName == "class Sonicteam::Player::State::SonicGemThrow") {
                    ProcessBothParts([&](auto& part) {
                        if (part.ESPState != 2) {
                            ApplyToAllSubAnims(part.pAnimationESPStart, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 0.0f, 0.8f, 1.0f);
                                });
                            ApplyToAllSubAnims(part.pAnimationESPLoop, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 0.0f, 0.8f, 1.0f);
                                });
                        }
                        });
                }
                // Green states
                else if (StateName == "class Sonicteam::Player::State::SonicTornadoGround" ||
                    StateName == "class Sonicteam::Player::State::SonicTornadoAir") {
                    ProcessBothParts([&](auto& part) {
                        if (part.ESPState != 2) {
                            ApplyToAllSubAnims(part.pAnimationESPStart, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 0.0f, 1.0f, 0.3f);
                                });
                            ApplyToAllSubAnims(part.pAnimationESPLoop, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 0.0f, 1.0f, 0.3f);
                                });
                        }
                        });
                }
                // White state
                else if (StateName == "class Sonicteam::Player::State::SonicHomingSmash") {
                    ProcessBothParts([&](auto& part) {
                        if (part.ESPState != 2) {
                            ApplyToAllSubAnims(part.pAnimationESPStart, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 1.0f, 1.0f, 1.0f);
                                });
                            ApplyToAllSubAnims(part.pAnimationESPLoop, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 1.0f, 1.0f, 1.0f);
                                });
                        }
                        });
                }
                // Red state
                else if (pSonicContext->m_SlowTime) {
                    ProcessBothParts([&](auto& part) {
                        if (part.ESPState != 2) {
                            ApplyToAllSubAnims(part.pAnimationESPStart, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 1.0f, 0.2f, 0.2f);
                                });
                            ApplyToAllSubAnims(part.pAnimationESPLoop, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 1.0f, 0.2f, 0.2f);
                                });
                        }
                        });
                }
                // Purple state
                else if (StateName == "class Sonicteam::Player::State::SonicBlastJump") {
                    ProcessBothParts([&](auto& part) {
                        if (part.ESPState != 2) {
                            ApplyToAllSubAnims(part.pAnimationESPStart, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 1.0f, 0.0f, 1.0f);
                                });
                            ApplyToAllSubAnims(part.pAnimationESPLoop, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 1.0f, 0.0f, 1.0f);
                                });
                        }
                        });
                }
                // Magenta state
                else if (pSonicContext->m_Shrink) {
                    ProcessBothParts([&](auto& part) {
                        if (part.ESPState != 2) {
                            ApplyToAllSubAnims(part.pAnimationESPStart, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 0.8f, 0.0f, 0.8f);
                                });
                            ApplyToAllSubAnims(part.pAnimationESPLoop, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 0.8f, 0.0f, 0.8f);
                                });
                        }
                        });
                }
                // Blue state
                else if (pSonicContext->m_MachAura) {
                    ProcessBothParts([&](auto& part) {
                        if (part.ESPState != 2) {
                            ApplyToAllSubAnims(part.pAnimationESPStart, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 0.2f, 0.4f, 1.0f);
                                });
                            ApplyToAllSubAnims(part.pAnimationESPLoop, [&](SubAnimation* anim) {
                                CreateHighResAnimation(anim);
                                PaintSolidAnim(anim, 0.2f, 0.4f, 1.0f);
                                });
                        }
                        });
                }
            }

            pModel->m_ESPShoes.EnableAura();
            pModel->m_ModelESP.EnableAura();
        }
        else
        {
            ProcessBothParts([&](Sonicteam::Player::ModelStandardESPPart& part)
                {
                    Color _color;
                    for (auto& it : part.pAnimationESPLoop->m_spMaterialAnimationObject->m_mspSubAnimations)
                    {
                        _color = *it.second->GetAnimationData<Color>();
                        break;
                    }
    
                    ApplyToAllSubAnims(part.pAnimationESPEnd, [&](SubAnimation* anim) {
                        CreateHighResAnimation(anim,12);
                        PaintPaletteAnim(anim, CreateFadeOutPalette(_color.r, _color.g, _color.b,12), 1.0f);
                        });
                });

            pModel->m_ESPShoes.DisableAura();
            pModel->m_ModelESP.DisableAura();
        }
    }

    if (Config::AlwaysBlueGem)
    {
        static const std::set<std::string> ALLOWED_MACH_SPEED_STATES = {
            "class Sonicteam::Player::State::CommonBrake",
            "class Sonicteam::Player::State::CommonQuickTurn",
            "class Sonicteam::Player::State::CommonGrind",
            "class Sonicteam::Player::State::CommonOnPathColli",
            "class Sonicteam::Player::State::SonicSliding",
            "class Sonicteam::Player::State::SonicAttack",
            "class Sonicteam::Player::State::SonicSpinDash"
        };

        auto currentStateName = stdx::VftableToRTTI(pCurrentState->m_pVftable)->typeDesc->name();
        auto gauge = pSonicContext->m_Gauge.get();

        if (ALLOWED_MACH_SPEED_STATES.contains(currentStateName) && GuestToHostFunction<bool>(::sub_82218898, pSonicContext, SonicContext::Gem_Blue))
        {
            if (currentStateName == "class Sonicteam::Player::State::CommonGrind")
            {
                auto pGrindState = static_cast<Sonicteam::Player::State::CommonGrind*>(pCurrentState);

                pSonicContext->m_MachAura = 1;
                pSonicContext->m_GimmickSpeedForward = pSonicContext->c_custom_action_machspeed_acc;
                pSonicContext->m_LockButtons = Config::DisableMachStateLockInput ? 0.0f : pSonicContext->c_custom_action_machspeed_time.get();
                pSonicContext->m_LastLockButtons = pSonicContext->m_LockButtons;
                pGrindState->m_PenaltyTime = pSonicContext->c_custom_action_machspeed_time + pSonicContext->c_grind_penalty_time;

                if (gauge) gauge->m_GroundedTime = -gauge->c_gauge_heal_delay;
            }
            else
            {
                pStateMachine->ChangeState(Sonicteam::Player::State::SonicState_MachSpeed);
            }
        }

        if (currentStateName == "class Sonicteam::Player::State::CommonGrind")
        {
            auto pGrindState = static_cast<Sonicteam::Player::State::CommonGrind*>(pCurrentState);
            float remainingPenaltyTime = pGrindState->m_PenaltyTime - pSonicContext->c_custom_action_machspeed_time;

            if (remainingPenaltyTime <= 0.0f && pSonicContext->m_MachAura)
            {
                pSonicContext->m_MachAura = 0;
                pSonicContext->m_GimmickSpeedForward = 0.0f;
            }
        }
    }
}

//Sonicteam::Player::State::CommonGrind::OnEnd2()
PPC_FUNC_IMPL(__imp__sub_82209660);
PPC_FUNC(sub_82209660)
{
    if (!Config::AlwaysBlueGem)
    {
        __imp__sub_82209660(ctx, base);
        return;
    }
     

    using SonicContext = Sonicteam::Player::State::SonicContext;
    auto pGrindState = (Sonicteam::Player::State::CommonGrind*)(base + ctx.r3.u32);
    auto pContext = (SonicContext*)(pGrindState->m_pContext.get());
    pContext->m_MachAura = 0;
    pContext->m_GimmickSpeedForward = 0;
    __imp__sub_82209660(ctx, base);
}


PPC_FUNC_IMPL(__imp__sub_822385E0);
PPC_FUNC(sub_822385E0)
{
    auto pModelSonic = (Sonicteam::Player::ModelSonic*)(base + ctx.r3.u32);
    if (pModelSonic->IPlugIn::m_pVftable.ptr.get() == 0x8200CFDC)
    {
        if (Config::EnableGemsAura)
            GuestToHostFunction<void>(sub_82232030, &pModelSonic->m_ESPShoes, ctx.f1.f64);

    }
    __imp__sub_822385E0(ctx, base);
}

bool SonicObject_OnUpgradeUpdate()
{
    return (Config::UnlockAllUpgrades);
}


bool ModelSonic_BlockGPFrameGPFix()
{
    return (Config::FixShoesParticleOffsetSwap);
}

bool ModelSonic_OnContextFlagGPFix(PPCRegister& model, PPCRegister& gem_index)
{
    if (!Config::FixShoesParticleOffsetSwap)
        return false;

    using SonicModel = Sonicteam::Player::ModelSonic;
    SonicModel* pModel = reinterpret_cast<SonicModel*>(g_memory.base + model.u32);
    auto* pLoad = pModel->m_pPlayerLoad.get();
    auto temp = pModel->m_pFrameGPShoes;

 
    // Save Particles From Previous Frame
    std::map<std::string, std::vector< Sonicteam::SoX::Graphics::FrameObserver*>> ParticleJointSave;
    std::map<std::string, xpointer<Sonicteam::Player::HairSimulator>> HairSimulatorSave;
    pModel->m_spRootFrame->ForEach([&](Sonicteam::SoX::Graphics::Frame* frame)
        {
            for (auto& it : frame->m_llFrameObserver)
            {
                if (stdx::ClassToName(it.m_pThis) == "class Sonicteam::Player::Effect::ParticleJoint")
                {
                    auto pJoint = (Sonicteam::Player::Effect::ParticleJoint*)(it.m_pThis.get());
                    pJoint->m_pParticle->Func1C(pModel->m_spRootFrame.get()); // Final Fix
                    ParticleJointSave[frame->GetName() == 0 ? "" : frame->GetName()].push_back(it.m_pThis.get());
                }

                if (stdx::ClassToName(it.m_pThis) == "class Sonicteam::Player::HairSimulator")
                {
                    HairSimulatorSave[frame->GetName() == 0 ? "" : frame->GetName()] = (Sonicteam::Player::HairSimulator*)it.m_pThis.get();
                }

            }
        });

    pModel->m_spPackageShoesModel->m_pFrameGPObject->RemoveFromParent();
    pModel->m_spPackageModel->m_pFrameGPObject->RemoveFromParent();
    pModel->m_spFrameGPStandard->AddChild(pModel->m_spPackageShoesModel->m_pFrameGPObject);


    // Save Animation Data
    auto frame = pModel->m_spPackageModel->m_PackageAnimationRoot->GetKeyFrame();

    // Swap Model
    GuestToHostFunction<void>(sub_82237470, pModel);

    // Restore Animation Data (ID already handled by sub_82237470)
    pModel->m_spPackageModel->m_PackageAnimationRoot->SetKeyFrame2(frame);

    // Detach Items
    for (auto& model : pModel->m_spItem->m_pvItemPoints)
    {
        model.m_spInstanceModel->m_spNode->RemoveFromParent();
    }

    // Try To Join Partiles to New Nodes
    for (auto it : ParticleJointSave)
    {
        auto NodeTo = pModel->m_spFrameGPStandard->FindNode(it.first.c_str());
        if (!NodeTo)
            NodeTo = pModel->m_spFrameGPStandard->m_pParent;

        for (auto& observer : it.second)
        {
            observer->m_lnFrameObserver.Reset();
            observer->m_pFrameGPStandard = 0;
            observer->m_pFrameGPStandard = NodeTo;
            NodeTo->m_llFrameObserver.Insert(&observer->m_lnFrameObserver);
        }
    }

    // Use Saved HairSimulator Data 
    pModel->m_spRootFrame->ForEach([&](Sonicteam::SoX::Graphics::Frame* frame)
        {
            auto NodeName = frame->GetName() == 0 ? "" : frame->GetName();

            if (HairSimulatorSave.find(NodeName) == HairSimulatorSave.end())
                return;

            auto HP = HairSimulatorSave[NodeName].get();

            for (auto& it : frame->m_llFrameObserver)
            {
                if (stdx::ClassToName(it.m_pThis) == "class Sonicteam::Player::HairSimulator")
                {
                    auto CP = (Sonicteam::Player::HairSimulator*)it.m_pThis.get();
                    CP->m_vField20 = HP->m_vField20;
                    CP->m_vField30 = HP->m_vField30;
                    CP->m_vField40 = HP->m_vField40;
                    CP->m_vField50 = HP->m_vField50;
                    CP->m_vField60 = HP->m_vField60;
                    CP->m_vField70 = HP->m_vField70;
                    CP->m_vField80 = HP->m_vField80;
                    CP->m_vField90 = HP->m_vField90;
                    CP->m_vFieldA0 = HP->m_vFieldA0;
                    CP->m_vFieldB0 = HP->m_vFieldB0;
                    CP->m_vFieldC0 = HP->m_vFieldC0;
                    CP->m_vFieldD0 = HP->m_vFieldD0;
                    CP->m_vFieldF0 = HP->m_vFieldF0;
                    CP->m_vField100 = HP->m_vField100;
                    CP->m_vField110 = HP->m_vField110;
                    CP->m_vField120 = HP->m_vField120;
                }

            }
        });

    // Attach Items to new Nodes
    for (auto& model : pModel->m_spItem->m_pvItemPoints)
    {
    
        if (auto Node = pModel->m_spFrameGPStandard->FindNode(model.m_AttachPoint.c_str()))
        {
            printf("node added %p to %p-%s\n",
                model.m_spInstanceModel->m_spNode.get(),
                Node,
                Node->GetName());

            Node->AddChild(model.m_spInstanceModel->m_spNode.get());
        }
    }

    return true;
}
