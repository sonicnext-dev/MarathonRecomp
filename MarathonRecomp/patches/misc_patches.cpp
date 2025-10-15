#include <api/Marathon.h>
#include <ui/black_bar.h>
#include <user/config.h>
#include <user/achievement_manager.h>
#include <app.h>

// TODO (Hyper): implement achievements menu.
void AchievementManagerUnlockMidAsmHook(PPCRegister& id)
{
    AchievementManager::Unlock(id.u32);
}

void SetLifeBarAnimation(PPCRegister& r3, PPCRegister& r4, PPCRegister& r5, PPCRegister& r31)
{
    static bool s_initContextualHUD{};
    static char* s_lifeBarSceneName{};

    if (!s_initContextualHUD)
    {
        constexpr const char* LIFE_BAR_ANIME = "life_bar_anime";

        s_lifeBarSceneName = (char*)g_userHeap.Alloc(strlen(LIFE_BAR_ANIME) + 1);

        strcpy(s_lifeBarSceneName, LIFE_BAR_ANIME);

        if (Config::RestoreContextualHUDColours)
        {
            *Sonicteam::Globals::ms_MainDisplayColours[Sonicteam::Character_Shadow] = 1.0f;
            *Sonicteam::Globals::ms_MainDisplayColours[Sonicteam::Character_Omega] = 1.0f;
            *Sonicteam::Globals::ms_MainDisplayColours[Sonicteam::Character_Rouge] = 1.0f;
            *Sonicteam::Globals::ms_MainDisplayColours[Sonicteam::Character_Silver] = 2.0f;
            *Sonicteam::Globals::ms_MainDisplayColours[Sonicteam::Character_Amy] = 2.0f;
            *Sonicteam::Globals::ms_MainDisplayColours[Sonicteam::Character_Blaze] = 2.0f;
        }

        s_initContextualHUD = true;
    }

    auto base = g_memory.base;
    auto pCsdObject = (Sonicteam::CsdObject*)(base + PPC_LOAD_U32(r3.u32));
    auto pSceneName = (const char*)(base + r4.u32);

    // Redirect "life_ber_anime" to "life_bar_anime", as they
    // actually spelt "bar" correctly in the tag XNCP scene names...
    if ((pCsdObject->m_pCsdResource->m_FilePath == "sprite/tagdisplay_1p" ||
        pCsdObject->m_pCsdResource->m_FilePath == "sprite/tagdisplay_2p") &&
        strcmp(pSceneName, "life_ber_anime") == 0)
    {
        r4.u32 = g_memory.MapVirtual(s_lifeBarSceneName);
    }

    if (!Config::RestoreContextualHUDColours)
        return;

    auto pHUDMainDisplay = (Sonicteam::HUDMainDisplay*)g_memory.Translate(r31.u32);

    switch (pHUDMainDisplay->m_Character)
    {
        case Sonicteam::Character_Sonic:
        case Sonicteam::Character_Tails:
        case Sonicteam::Character_Knuckles:
            r5.u32 = 0x82036778; // "sonic_in"
            break;

        case Sonicteam::Character_Shadow:
        case Sonicteam::Character_Omega:
        case Sonicteam::Character_Rouge:
            r5.u32 = 0x8203676C; // "shadow_in"
            break;

        case Sonicteam::Character_Silver:
        case Sonicteam::Character_Amy:
        case Sonicteam::Character_Blaze:
            r5.u32 = 0x82036760; // "silver_in"
            break;
    }
}

void SetRingBarIndex(PPCRegister& r5, PPCRegister& r31)
{
    if (!Config::RestoreContextualHUDColours)
        return;

    auto pHUDMainDisplay = (Sonicteam::HUDMainDisplay*)g_memory.Translate(r31.u32);

    r5.u32 = pHUDMainDisplay->m_Character;
}

// Redirects 2P HUD to 1P HUD to remove overscan compensation.
void Load2PDisplayMidAsmHook() {}

void PostureDisableEdgeGrabLeftover(PPCRegister& posture)
{
    if (!Config::DisableEdgeGrabLeftover)
        return;

    auto base = g_memory.base;

    *(volatile uint8_t*)(base + (posture.u32 + 0x3C0)) = 1;
}

void PedestrianAnimationLOD(PPCRegister& val)
{
    val.u32 = 0;
}

// Sonicteam::CommonObjectHint::Update
PPC_FUNC_IMPL(__imp__sub_822CE930);
PPC_FUNC(sub_822CE930)
{
    auto pCommonObjectHint = (Sonicteam::CommonObjectHint*)(base + ctx.r3.u32);

    if (!Config::Hints && pCommonObjectHint->m_Type == Sonicteam::CommonObjectHint::CommonObjectHintType_HintRing)
    {
        pCommonObjectHint->Destroy();
        return;
    }

    if (!Config::ControlTutorial)
    {
        guest_stack_var<int> stack{};

        auto pspTextCard = GuestToHostFunction<boost::shared_ptr<Sonicteam::TextCard>*>(sub_825ECB48,
            stack.get(), App::s_pApp->GetGame()->m_pHintTextBook.get(), (const char*)&pCommonObjectHint->m_MessageName);

        if (auto pTextCard = pspTextCard->get())
        {
            if (pTextCard->m_pVariables && strstr(pTextCard->m_pVariables, "picture(button_"))
            {
                pCommonObjectHint->Destroy();
                return;
            }
        }
    }

    __imp__sub_822CE930(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_824A6EA8);
PPC_FUNC(sub_824A6EA8)
{
    if (Config::SkipIntroLogos && ctx.r4.u32 == 1)
        ctx.r4.u32 = 4;

    __imp__sub_824A6EA8(ctx, base);
}

PPC_FUNC(sub_82188460)
{
    ctx.r3.u64 = Config::Subtitles;
}

void NOP() {}
