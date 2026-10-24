#include <api/Marathon.h>
#include <kernel/memory.h>
#include <patches/aspect_ratio_patches.h>
#include <ui/options_menu.h>
#include <app.h>

// Sonicteam::PauseAdapter::ProcMsgGetItemID
PPC_FUNC_IMPL(__imp__sub_8216DA08);
PPC_FUNC(sub_8216DA08)
{
    const auto pPauseAdapter = reinterpret_cast<Sonicteam::PauseAdapter*>(base + ctx.r3.u32);
    const auto pMsgGetText = reinterpret_cast<Sonicteam::Message::PauseAdapter::MsgGetText*>(base + ctx.r4.u32);

    __imp__sub_8216DA08(ctx, base);

    // Set selected ID to unused slot.
    if (pMsgGetText->SelectedName == "options")
        pPauseAdapter->m_SelectedID = Sonicteam::PauseAdapter::PauseItemID(6);
}

// Sonicteam::PauseAdapter::ProcMsgItemSelected
PPC_FUNC_IMPL(__imp__sub_82170E48);
PPC_FUNC(sub_82170E48)
{
    const auto pPauseAdapter = reinterpret_cast<Sonicteam::PauseAdapter*>(base + ctx.r3.u32);

    if (pPauseAdapter->m_SelectedID == 6)
    {
        OptionsMenu::s_pBgmCue = static_cast<Sonicteam::GameImp*>(pPauseAdapter->m_pOwner.get())->GetBgmCue();
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
    const auto pPauseTask = reinterpret_cast<Sonicteam::PauseTask*>(base + ctx.r3.u32);

    static bool isReturningFromOptionsMenu{};
    static int optionsItemIndex{};

    switch (pPauseTask->m_State)
    {
        case Sonicteam::PauseTask::PauseTaskState_Opened:
        {
            optionsItemIndex = pPauseTask->m_ItemCount - 1;
            
            for (int i = 0; i < pPauseTask->m_ItemNames.Capacity(); i++)
            {
                const auto& rItemName = pPauseTask->m_ItemNames[i];
            
                // Ensure Options item is always above the quit items.
                if (rItemName == "quit" || rItemName == "return" || rItemName == "return_to_select")
                {
                    optionsItemIndex = i;
                    break;
                }
            }
            
            pPauseTask->Insert(optionsItemIndex, "options", App::s_pApp->GetGame()->m_pSystemTextBook.get(), "msg_options");

            break;
        }

        case Sonicteam::PauseTask::PauseTaskState_Opening:
        case Sonicteam::PauseTask::PauseTaskState_Idle:
        {
            if (!isReturningFromOptionsMenu)
                break;

            // Set cursor to Options.
            pPauseTask->m_SelectedIndex = optionsItemIndex;

            isReturningFromOptionsMenu = false;

            break;
        }

        case Sonicteam::PauseTask::PauseTaskState_Closed:
        {
            if (OptionsMenu::s_isVisible)
            {
                if (OptionsMenu::s_state == OptionsMenuState::Closing)
                {
                    pPauseTask->m_State = Sonicteam::PauseTask::PauseTaskState_Opened;
                    isReturningFromOptionsMenu = true;
                }
                else
                {
                    return;
                }
            }

            break;
        }
    }

    if (Config::RestoreLoadingTransition || Config::RestorePauseMissionText)
    {
        App::s_pApp->m_pDoc->m_pRootTask->WalkSiblings([&](Sonicteam::SoX::Engine::Task* in_pTask) -> bool
        {
            const auto pTaskName = in_pTask->GetName();

            if (Config::RestoreLoadingTransition && strcmp(pTaskName, "HUDLoading") == 0)
            {
                in_pTask->Update(ctx.f1.f64);
            }
            else if (Config::RestorePauseMissionText && strcmp(pTaskName, "HUDMessageWindow") == 0)
            {
                switch (pPauseTask->m_State)
                {
                    case Sonicteam::PauseTask::PauseTaskState_Opening:
                    {
                        // Update message window for closing animation.
                        in_pTask->Update(ctx.f1.f64);
                        break;
                    }

                    case Sonicteam::PauseTask::PauseTaskState_Closed:
                    {
                        // Restore message window upon unpausing.
                        guest_stack_var<Sonicteam::Message::HUDMessageWindow::MsgChangeState> msgChangeState(0);
                        in_pTask->ProcessMessage(msgChangeState.get());
                        break;
                    }
                }
            }
            else
            {
                return true;
            }

            return false;
        });
    }

    SetTextEntityModifier(pPauseTask->m_spMissionTextEntity.get(), CSD_ALIGN_BOTTOM | CSD_SCALE);

    __imp__sub_82509870(ctx, base);
}

// Skips the original code that sends this message, as it's sent *before* the
// text entity for the mission window is created, resulting in the message processor
// seeing nullptr. We'll send this message ourselves after the text entity is created.
bool PauseTask_SkipSendMsgChangeState()
{
    return Config::RestorePauseMissionText;
}

void PauseTask_ConfigureMissionWindow(PPCRegister& r31)
{
    const auto pPauseTask = reinterpret_cast<Sonicteam::PauseTask*>(g_memory.Translate(r31.u32));

    // The mission text is drawn at a lower priority than the mission window by default.
    // 1001.0f is the priority value used by the rest of the pause menu.
    if (pPauseTask->m_spMissionTextEntity)
        GuestToHostFunction<int>(sub_8262AF00, pPauseTask->m_spMissionTextEntity.get(), 1001.0f);

    if (!Config::RestorePauseMissionText)
        return;

    auto showMissionWindow = true;

    if (!pPauseTask->m_spMissionTextEntity || pPauseTask->m_spMissionTextEntity->m_TextLength <= 0)
        showMissionWindow = false;

    auto msgChangeState = guest_stack_var<Sonicteam::Message::HUDPause::MsgChangeState>(0, pPauseTask->m_Field78.get());
    pPauseTask->m_pHUDPause->m_ShowMissionWindow = showMissionWindow;
    pPauseTask->m_pHUDPause->ProcessMessage(msgChangeState.get());
}
