#include <api/Marathon.h>
#include <hid/hid.h>
#include <os/locale.h>
#include <patches/aspect_ratio_patches.h>
#include <ui/imgui_utils.h>
#include <user/config.h>
#include <app.h>

// Sonicteam::SoX::Graphics::Xenon::DeviceXenon::DrawText
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
    const auto x = float(ctx.r5.u32);
    const auto y = float(ctx.r6.u32);
    const auto colour = reinterpret_cast<Sonicteam::SoX::ARGB8888*>(base + ctx.r7.u32);

    Video::EnqueueImGuiDraw([=]()
    {
        ImGui::GetBackgroundDrawList()->AddText
        (
            g_pFntArialUni,
            Scale(12),
            { AspectRatioPatches::AlignX(Scale(x)), Scale(y) },
            IM_COL32(colour->R(), colour->G(), colour->B(), colour->A()),
            text.c_str()
        );
    });
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
                        auto pGame = App::s_pApp->GetGame();
                        auto pZock = pPlayer->GetPlugIn<Sonicteam::Player::Zock>("zock");
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
        if (Config::RestoreDemoCameraMode && pInputManager->PadState.IsPressed(Sonicteam::SoX::Input::PadButton_RightStick))
        {
            auto pCameraman = static_cast<Sonicteam::Camera::Cameraman*>(pPlayer->m_pCameraman.get());

            if (auto pCameraMode = pCameraman->m_spCameraModeManager->m_spCameraMode.get())
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

    if (pPadState->IsPressed(Sonicteam::SoX::Input::PadButton_Back))
    {
        LOGN("Debug Mode: Disabled");
        return true;
    }

    return false;
}
