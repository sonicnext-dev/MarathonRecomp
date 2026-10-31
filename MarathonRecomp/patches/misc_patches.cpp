#include <api/Marathon.h>
#include <hid/hid.h>
#include <user/config.h>
#include <user/achievement_manager.h>
#include <app.h>

void UnlockAchievement(PPCRegister& id)
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
    if ((pCsdObject->m_pCsdResource->m_MgrResourceName == "sprite/tagdisplay_1p" ||
        pCsdObject->m_pCsdResource->m_MgrResourceName == "sprite/tagdisplay_2p") &&
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
    const auto pCommonObjectHint = reinterpret_cast<Sonicteam::CommonObjectHint*>(base + ctx.r3.u32);
    const auto pGame = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::GameMode>()->GetGame();

    const auto& rMessageName = pCommonObjectHint->m_MessageName;

    if (pCommonObjectHint->m_Type == Sonicteam::CommonObjectHint::CommonObjectHintType_HintRing)
    {
        bool isHintRingBlocked{};

        if (Config::Hints)
        {
            auto& rTextCards = pGame->m_pHintTextBook->m_TextCards;

            const auto it = rTextCards.find(rMessageName);

            if (it != rTextCards.end())
            {
                const auto& spTextCard = it->second;

                if (spTextCard->m_pVariables)
                {
                    // Block controller hint rings.
                    if (strstr(spTextCard->m_pVariables, "picture(button_"))
                        isHintRingBlocked = !Config::ControlTutorial;
                }
            }
        }
        else
        {
            // Block all hint rings.
            isHintRingBlocked = true;
        }

        if (isHintRingBlocked)
        {
            pCommonObjectHint->Destroy();
            return;
        }
    }

    if (pGame->m_pMissionCore)
    {
        struct HintFlagTest
        {
            std::string_view MessageName{};
            uint32_t GlobalFlag{};
            std::function<bool()> Condition{};
        };

        // TODO: unblock these hints for action remapping and just mute them instead.
        static const std::array<HintFlagTest, 3> hintFlagTests =
        {
            // Block "hint_twn01_e01_tl" if Sonic's Light Dash is unlocked.
            HintFlagTest{ "hint_twn01_e00_tl", 6000, []() { return !Config::ControlTutorial || Config::LightDash != ELightDash::X; } },

            // Block "hint_all03_h26_so" if Sonic's Antigravity is unlocked.
            HintFlagTest{ "hint_twn01_e02_tl", 6001, []() { return !Config::ControlTutorial || Config::SlidingAttack != ESlidingAttack::X; } },

            // Block "hint_twn01_e44_rg" if Shadow's Light Dash is unlocked.
            HintFlagTest{ "hint_twn01_e43_rg", 6012, []() { return !Config::ControlTutorial || Config::LightDash != ELightDash::X; } }
        };

        for (const auto& rTest : hintFlagTests)
        {
            if (!rTest.Condition() || rTest.MessageName != rMessageName)
                continue;

            auto msgGetGlobalFlag = guest_stack_var<Sonicteam::Message::Mission::MsgGetGlobalFlag>(rTest.GlobalFlag);
            pGame->m_pMissionCore->ProcessMessage(msgGetGlobalFlag.get());

            if (msgGetGlobalFlag->FlagValue == 0)
                break;

            pCommonObjectHint->Destroy();

            return;
        }
    }

    __imp__sub_822CE930(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_8244D288);
PPC_FUNC(sub_8244D288)
{
    auto isShadowEggCerberus = PPC_LOAD_U32(ctx.r3.u32 + 0x1C);

    // Prevent Eggman's voice line playing
    // for Shadow's variant of Egg Cerberus.
    if (!Config::Hints && isShadowEggCerberus)
        return;

    __imp__sub_8244D288(ctx, base);
}

bool Super3_DisableChangeRequestHint()
{
    return !Config::ControlTutorial;
}

PPC_FUNC_IMPL(__imp__sub_824A6EA8);
PPC_FUNC(sub_824A6EA8)
{
    if ((App::s_isSkipLogos || Config::BootDirectlyToTitle) && ctx.r4.u32 == 1)
        ctx.r4.u32 = 4;

    __imp__sub_824A6EA8(ctx, base);
}

PPC_FUNC(sub_82188460)
{
    ctx.r3.u64 = Config::Subtitles;
}

void NOP() {}
