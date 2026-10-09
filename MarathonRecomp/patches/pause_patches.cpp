#include <api/Marathon.h>
#include <kernel/memory.h>
#include <patches/aspect_ratio_patches.h>
#include <ui/options_menu.h>
#include <app.h>

void AddPauseMenuItem
(
    Sonicteam::TextBook* in_pTextBook,
    stdx::vector<stdx::string>* in_pvActionNames,
    stdx::vector<Sonicteam::TextCard>* in_pvTextCards,
    stdx::vector<int>* in_pvEnabledItems,
    const char* in_pActionName,
    const char* in_pTextName,
    bool in_isEnabled
)
{
    guest_stack_var<stdx::string> actionName(in_pActionName);
    guest_stack_var<stdx::string> textName(in_pTextName);
    guest_stack_var<boost::shared_ptr<Sonicteam::TextCard>> spTextCard;
    guest_stack_var<int> isEnabled((int)in_isEnabled);

    GuestToHostFunction<int>(sub_8217D608, in_pvActionNames, actionName.get());
    GuestToHostFunction<int>(sub_825ECB48, spTextCard.get(), in_pTextBook, textName->c_str());
    GuestToHostFunction<int>(sub_8239E8F0, in_pvTextCards, spTextCard.get());
    GuestToHostFunction<int>(sub_823879C8, in_pvEnabledItems, isEnabled.get());
}

void GameImp_PauseMenu_AddQuitPrefix(PPCRegister& r1, PPCRegister& r30)
{
    auto pGameImp = (Sonicteam::GameImp*)g_memory.Translate(r30.u32);
    auto pvActionNames = (stdx::vector<stdx::string>*)g_memory.Translate(r1.u32 + 0x70);
    auto pvTextCards = (stdx::vector<Sonicteam::TextCard>*)g_memory.Translate(r1.u32 + 0x60);
    auto pvEnabledItems = (stdx::vector<int>*)g_memory.Translate(r1.u32 + 0x80);

    AddPauseMenuItem(pGameImp->m_pSystemTextBook, pvActionNames, pvTextCards, pvEnabledItems, "options", "msg_options", true);
}

// Sonicteam::PauseAdapter::MapActionNameToID (speculatory)
PPC_FUNC_IMPL(__imp__sub_8216DA08);
PPC_FUNC(sub_8216DA08)
{
    auto pPauseAdapter = (Sonicteam::PauseAdapter*)(base + ctx.r3.u32);
    auto pMsgGetText = (Sonicteam::Message::PauseAdapter::MsgGetText*)(base + ctx.r4.u32);

    __imp__sub_8216DA08(ctx, base);

    // Set selected ID to unused slot.
    if (pMsgGetText->SelectedName == "options")
        pPauseAdapter->m_SelectedID = 6;
}

// Sonicteam::PauseAdapter::DoAction (speculatory)
PPC_FUNC_IMPL(__imp__sub_82170E48);
PPC_FUNC(sub_82170E48)
{
    auto pPauseAdapter = (Sonicteam::PauseAdapter*)(base + ctx.r3.u32);

    if (pPauseAdapter->m_SelectedID == 6)
    {
        OptionsMenu::s_pBgmCue = pPauseAdapter->GetGame()->GetBgmCue();
        OptionsMenu::Open(true);
        return;
    }

    __imp__sub_82170E48(ctx, base);
}

// Sonicteam::HUDPause::ProcessMessage
PPC_FUNC_IMPL(__imp__sub_824F05D8);
PPC_FUNC(sub_824F05D8)
{
    if (!Config::RestorePauseMissionText)
    {
        __imp__sub_824F05D8(ctx, base);
        return;
    }

    const auto pHUDPause = static_cast<Sonicteam::HUDPause*>(reinterpret_cast<Sonicteam::SoX::MessageReceiver*>(base + ctx.r3.u32));
    const auto pMessage = reinterpret_cast<Sonicteam::SoX::IMessage*>(base + ctx.r4.u32);
    
    pHUDPause->m_ShowMissionWindow = true;
    
    if (pMessage->ID == Sonicteam::Message::HUDPause::MsgChangeState::GetID())
    {
        const auto pMsgChangeState = static_cast<Sonicteam::Message::HUDPause::MsgChangeState*>(pMessage);

        if (pMsgChangeState->State == 0)
        {
            App::s_pApp->m_pDoc->m_pRootTask->WalkSiblings([](Sonicteam::SoX::Engine::Task* in_pTask) -> bool
            {
                if (strcmp(in_pTask->GetName(), "HUDMessageWindow") != 0)
                    return true;

                const auto pHUDMessageWindow = static_cast<Sonicteam::HUDMessageWindow*>(in_pTask);

                // Close message window upon pausing.
                guest_stack_var<Sonicteam::Message::HUDMessageWindow::MsgChangeState> msgChangeState(2);
                pHUDMessageWindow->ProcessMessage(msgChangeState.get());

                return false;
            });
        }
    }

    __imp__sub_824F05D8(ctx, base);
}

// Sonicteam::PauseTask::Update
PPC_FUNC_IMPL(__imp__sub_82509870);
PPC_FUNC(sub_82509870)
{
    auto pPauseTask = (Sonicteam::PauseTask*)(base + ctx.r3.u32);

    static bool s_isReturningFromOptionsMenu{};

    switch (pPauseTask->m_State)
    {
        case Sonicteam::PauseTask::PauseTaskState_Opening:
        case Sonicteam::PauseTask::PauseTaskState_Idle:
        {
            if (!s_isReturningFromOptionsMenu)
                break;

            // Set cursor to Options (should always be above the last item).
            pPauseTask->m_SelectedIndex = pPauseTask->m_ItemCount - 2;

            s_isReturningFromOptionsMenu = false;

            break;
        }

        case Sonicteam::PauseTask::PauseTaskState_Closed:
        {
            if (OptionsMenu::s_isVisible)
            {
                if (OptionsMenu::s_state == OptionsMenuState::Closing)
                {
                    pPauseTask->m_State = Sonicteam::PauseTask::PauseTaskState_Opened;
                    s_isReturningFromOptionsMenu = true;
                }
                else
                {
                    return;
                }
            }

            break;
        }
    }

    if (Config::RestorePauseMissionText)
    {
        SetTextEntityModifier(pPauseTask->m_pMissionText.get(), CSD_ALIGN_BOTTOM | CSD_SCALE);

        App::s_pApp->m_pDoc->m_pRootTask->WalkSiblings([&](Sonicteam::SoX::Engine::Task* in_pTask) -> bool
        {
            if (strcmp(in_pTask->GetName(), "HUDMessageWindow") != 0)
                return true;

            const auto pHUDMessageWindow = static_cast<Sonicteam::HUDMessageWindow*>(in_pTask);

            switch (pPauseTask->m_State)
            {
                case Sonicteam::PauseTask::PauseTaskState_Opening:
                {
                    // Update message window for closing animation.
                    pHUDMessageWindow->Update(ctx.f1.f64);
                    break;
                }

                case Sonicteam::PauseTask::PauseTaskState_Closed:
                {
                    // Restore message window upon unpausing.
                    guest_stack_var<Sonicteam::Message::HUDMessageWindow::MsgChangeState> msgChangeState(0);
                    pHUDMessageWindow->ProcessMessage(msgChangeState.get());
                    break;
                }
            }

            return false;
        });
    }

    __imp__sub_82509870(ctx, base);
}

// The mission text is drawn at a lower priority
// than the mission box by default. 1001.0f is the
// priority value used by the rest of the pause menu.
void PauseTask_SetMissionTextPriority(PPCRegister& priority)
{
    priority.f64 = 1001.0f;
}
