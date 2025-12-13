#include <api/Liberty.h>
#include <kernel/memory.h>
#include <ui/options_menu.h>

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
    auto pMsgPauseAdapterText = (Sonicteam::Message::MsgPauseAdapterText*)(base + ctx.r4.u32);

    __imp__sub_8216DA08(ctx, base);

    // Set selected ID to unused slot.
    if (pMsgPauseAdapterText->SelectedName == "options")
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

    __imp__sub_82509870(ctx, base);
}
