#include <api/Marathon.h>
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

// Sonicteam::PauseAdapter::MapActionNameToIndex (speculatory)
PPC_FUNC_IMPL(__imp__sub_8216DA08);
PPC_FUNC(sub_8216DA08)
{
    auto pPauseAdapter = (Sonicteam::PauseAdapter*)(base + ctx.r3.u32);
    auto pMsgPauseAdapterText = (Sonicteam::Message::MsgPauseAdapterText*)(base + ctx.r4.u32);

    __imp__sub_8216DA08(ctx, base);

    // Set selected index to unused slot.
    if (pMsgPauseAdapterText->SelectedName == "options")
        pPauseAdapter->m_SelectedIndex = 6;
}

void PauseAdapter_DoAction(PPCRegister& r11)
{
    switch (r11.u32)
    {
        case 6:
            OptionsMenu::Open(true);
            break;
    }
}
