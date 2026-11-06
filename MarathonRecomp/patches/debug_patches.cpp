#include <api/Marathon.h>
#include <hid/hid.h>
#include <os/locale.h>
#include <patches/aspect_ratio_patches.h>
#include <ui/imgui_utils.h>
#include <user/config.h>
#include <app.h>

static bool g_isStageSelectMultiplayer{};

PPC_FUNC_IMPL(__imp__sub_8259A638);
PPC_FUNC(sub_8259A638)
{
    // Scale debug menu text before handing off to device.
    ctx.r5.u32 = AspectRatioPatches::AlignX(Scale(ctx.r5.u32));
    ctx.r6.u32 = Scale(ctx.r6.u32);

    __imp__sub_8259A638(ctx, base);
}

// Sonicteam::SoX::Graphics::Xenon::DeviceXenon::DrawText
// TODO: check if this is called anywhere else besides the debug menu, as
// restoring this could display debug text anywhere else in the game too.
PPC_FUNC(sub_8289CB70)
{
    const auto pText = reinterpret_cast<const char*>(base + ctx.r4.u32);

    auto pTextSeek = pText;

    while (*pTextSeek++);

    uint32_t textLengthInChars = pTextSeek - pText;
    uint32_t textLengthInCharsNoTerminator = textLengthInChars - 1;

    if (textLengthInChars > 0x7FFFFFFF || !textLengthInCharsNoTerminator)
        return;

    const auto text = os::locale::ConvertToUTF8(os::locale::Encoding::ShiftJIS, std::string(pText, textLengthInCharsNoTerminator));
    const auto position = ImVec2(ctx.r5.u32, ctx.r6.u32);

    auto color = IM_COL32_WHITE;

    if (ctx.r7.u32)
    {
        const auto pColor = reinterpret_cast<Sonicteam::SoX::ARGB8*>(base + ctx.r7.u32);

        color = IM_COL32(pColor->R(), pColor->G(), pColor->B(), pColor->A());
    }

    Video::EnqueueImGuiDraw([=]()
    {
        ImGui::GetBackgroundDrawList()->AddText(g_pFntArialUni, Scale(12), position, color, text.c_str());
    });
}

void StageSelectMode_SetMultiplayerGameMode(PPCRegister& r3)
{
    const auto pStageSelectMode = reinterpret_cast<Sonicteam::StageSelectMode*>(g_memory.Translate(r3.u32));

    const auto selectedStageParams = std::string_view(pStageSelectMode->m_pSelectedStageMap->m_Name.c_str());
    const auto delimiterPos = selectedStageParams.find(':');

    if (delimiterPos == std::string_view::npos)
        return;

    // Splits on the colon delimiter to get the Lua script path.
    // The left hand side is the area path, and the right hand side is the script path.
    const auto missionPath = selectedStageParams.substr(delimiterPos + 1);

    const auto pResourceManager = Sonicteam::SoX::ResourceManager::GetInstance();
    const auto pMissionScript = pResourceManager->LoadResource<Sonicteam::LuaSystem, Sonicteam::LuaSystemManager>(missionPath.data());

    if (!pMissionScript)
        return;

    if (const auto pMissionInfoTable = pMissionScript->GetTableNode("g_mission_information"))
    {
        if (const auto pRule = pMissionInfoTable->GetString("mission_userule"))
        {
            const auto pDoc = reinterpret_cast<Sonicteam::DocMarathonState*>(pStageSelectMode->m_pDoc.get());

            if (const auto pGameRuleContext = pDoc->m_pGameRuleContext)
            {
                if (strstr(pRule, "tag"))
                {
                    pGameRuleContext->m_GameMode = Sonicteam::GameRule::Context::GameMode_Tag;
                }
                else if (strstr(pRule, "battle"))
                {
                    pGameRuleContext->m_GameMode = Sonicteam::GameRule::Context::GameMode_Battle;
                }

                if (pGameRuleContext->m_GameMode != Sonicteam::GameRule::Context::GameMode_None)
                    g_isStageSelectMultiplayer = true;
            }
        }
    }

    pMissionScript->Destroy();
}

void StageSelectMode_SetMultiplayerContext(PPCRegister& r3, PPCRegister& r4)
{
    if (!g_isStageSelectMultiplayer)
        return;

    const auto pGame = reinterpret_cast<Sonicteam::GameImp*>(g_memory.Translate(r3.u32));
    const auto pDoc = reinterpret_cast<Sonicteam::DocMarathonState*>(pGame->m_pDoc.get());
    
    int i{};

    // GameRule::Context requires the player names to be
    // populated in order for specific game modes to work.
    for (const auto& rPlayerInfo : pGame->m_PlayerInfo)
    {
        if (i >= pDoc->m_pGameRuleContext->m_PlayerNames.Capacity())
            break;

        if (rPlayerInfo.StartPlayer.empty())
            continue;

        auto name = std::string(rPlayerInfo.StartPlayer.c_str());

        // Infer player name from Lua script.
        if (const auto extensionPos = name.find(".lua"); extensionPos != std::string::npos)
            name = name.substr(0, extensionPos);

        pDoc->m_pGameRuleContext->m_PlayerNames[i] = name.c_str();

        i++;
    }

    r4.u32 = pGame->GetPlayerCount();

    g_isStageSelectMultiplayer = false;
}

// Sonicteam::DocMarathonState::StartMainMode
PPC_FUNC_IMPL(__imp__sub_82161AB8);
PPC_FUNC(sub_82161AB8)
{
    if (!Config::BootDirectlyToDevTitle)
    {
        __imp__sub_82161AB8(ctx, base);
        return;
    }

    const auto pDoc = reinterpret_cast<Sonicteam::DocMarathonState*>(base + ctx.r3.u32);

    GuestToHostFunction<int>(sub_825E9E28, ctx.r3.u32, ctx.r4.u32);

    ctx.r3.u32 = g_memory.MapVirtual(pDoc->SetDocState(StateDevTitle::GetInstance()));
}

// Sonicteam::Player::Object::Update
PPC_FUNC_IMPL(__imp__sub_82195500);
PPC_FUNC(sub_82195500)
{
    const auto pPlayer = reinterpret_cast<Sonicteam::Player::Object*>(base + ctx.r3.u32);
    const auto pInputManager = pPlayer->GetInputManager();

    if (pPlayer->m_IsPlayer && pInputManager)
    {
        if (Config::EnableDebugMode)
        {
            switch (pPlayer->m_SetupModuleIndexPostfix)
            {
                case 1:
                {
                    // Toggle debug posture on Select press.
                    if (pPlayer->m_SetupModuleIndexPrefix == 1 && pInputManager->PadState.IsPressed(Sonicteam::SoX::Input::PadButton_Back))
                    {
                        pPlayer->m_SetupModuleIndexPostfix = 2;
                        LOGN("Debug Mode: Enabled");
                    }

                    break;
                }

                case 2:
                {
                    // Toggle camera volume collision on B press.
                    if (pInputManager->PadState.IsPressed(Sonicteam::SoX::Input::PadButton_East))
                    {
                        const auto pGame = App::s_pApp->GetGame();
                        const auto pZock = pPlayer->GetPlugIn<Sonicteam::Player::Zock>("zock");
                        const auto collisionFilterInfo = pZock->m_spPhantomA->m_pRigidBody->m_collidable.m_broadPhaseHandle.m_collisionFilterInfo == 6 ? 0x383 : 6;

                        LOGFN("Camera Volumes: {}", collisionFilterInfo != 6 ? "Enabled" : "Disabled");

                        pZock->m_spPhantomA->m_pRigidBody->m_collidable.m_broadPhaseHandle.m_collisionFilterInfo = collisionFilterInfo;
                        pGame->GetPhysicsWorld<Sonicteam::SoX::Physics::Havok::WorldHavok>()->m_pWorld->updateCollisionFilterOnWorld(1, 1);
                    }

                    break;
                }
            }
        }

        // Toggle demo camera on right stick press.
        if (Config::RestoreDemoCameraMode && pInputManager->PadState.IsPressed(Sonicteam::SoX::Input::PadButton_RightStick))
        {
            const auto pCameraman = static_cast<Sonicteam::Camera::Cameraman*>(pPlayer->m_pCameraman.get());

            if (const auto pCameraMode = pCameraman->m_spCameraModeManager->m_spCameraMode.get())
            {
                guest_stack_var<Sonicteam::Message::Camera::Cameraman::MsgChangeMode> msgChangeMode;
                msgChangeMode->PadID = pInputManager->ID;
                msgChangeMode->TargetActorID = pPlayer->m_ActorID;
                msgChangeMode->IsDemoCamera = pCameraMode->m_pVftable.ptr != 0x82002004;

                LOGFN("Demo Camera: {}", msgChangeMode->IsDemoCamera ? "Enabled" : "Disabled");

                pCameraman->ProcessMessage(msgChangeMode.get());
            }
        }
    }

    __imp__sub_82195500(ctx, base);
}

// Add missing SetupModuleDebug to table.
void PlayerDebugMode_RegisterLuaSetup(PPCRegister& str, PPCRegister& index)
{
    if (!Config::EnableDebugMode)
        return;

    const auto pString = reinterpret_cast<stdx::string*>(g_memory.Translate(str.u32));

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
    const auto pPadState = reinterpret_cast<Sonicteam::SoX::Input::PadState*>(g_memory.Translate(r30.u32));

    if (pPadState->IsPressed(Sonicteam::SoX::Input::PadButton_Back))
    {
        LOGN("Debug Mode: Disabled");
        return true;
    }

    return false;
}
