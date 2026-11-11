#include <api/Marathon.h>
#include <install/update_checker.h>
#include <os/logger.h>
#include <ui/fader.h>
#include <ui/message_window.h>
#include <ui/options_menu.h>
#include <user/achievement_manager.h>
#include <user/config.h>
#include <user/paths.h>
#include <app.h>
#include <exports.h>
#include <functional>

constexpr auto TITLE_OPTION_OUTRO_FRAMES = (1.0 / 60.0) * 45.0;
constexpr auto TITLE_OUTRO_FRAMES = (1.0 / 60.0) * 25.0;
constexpr auto TITLE_OPTION_OUTRO_TOTAL_FRAMES = TITLE_OPTION_OUTRO_FRAMES + TITLE_OUTRO_FRAMES;

static double g_titleProceedOutroTime{};

struct YesNoMessage
{
    const char* pLocaleKey{};
    double DelayTime{};
    double DelayDuration{};
    int DefaultIndex{};
    std::function<void()> YesCallback{};
    std::function<void()> NoCallback{};
    bool IsOpen{};
    int Result{ -1 };
}
g_yesNoMessage{};

static bool g_ignoreDelayPreference{};
static bool g_saveDataExists{};

enum class DevTitleState
{
    Idle,
    Open,
    Start
};

static DevTitleState g_devTitleState{};
static uint32_t g_secretFlags{};

enum
{
    SECRET_NONE,
    SECRET_UP = 1 << 0,
    SECRET_DOWN = 1 << 1,
    SECRET_LEFT = 1 << 2,
    SECRET_RIGHT = 1 << 3,
    SECRET_ALL = SECRET_UP | SECRET_DOWN | SECRET_LEFT | SECRET_RIGHT
};

// TODO (Hyper): make the MessageWindow API less stupid.
static bool ProcessYesNoMessage()
{
    static bool isFading{};

    if (isFading)
        return true;

    if (!g_yesNoMessage.IsOpen)
        return false;

    if ((App::s_time - g_yesNoMessage.DelayTime) < g_yesNoMessage.DelayDuration)
        return true;

    static std::array<std::string, 2> options = { Localise("Common_Yes"), Localise("Common_No") };

    if (MessageWindow::Open(Localise(g_yesNoMessage.pLocaleKey), &g_yesNoMessage.Result, options, g_yesNoMessage.DefaultIndex) == MSG_CLOSED)
    {
        if (g_yesNoMessage.Result == 0)
        {
            Fader::FadeOut(1, [&]() { g_yesNoMessage.YesCallback(); });
            isFading = true;
        }
        else
        {
            g_yesNoMessage.NoCallback();
        }

        g_yesNoMessage.IsOpen = false;
        g_yesNoMessage.Result = -1;
    }

    return true;
}

static void OpenYesNoMessage(const char* in_pLocaleKey, double in_delayDuration, int in_defaultIndex, const std::function<void()>& in_rYesCallback, const std::function<void()>& in_rNoCallback)
{
    g_yesNoMessage =
    {
        .pLocaleKey = in_pLocaleKey,
        .DelayTime = App::s_time,
        .DelayDuration = in_delayDuration,
        .DefaultIndex = in_defaultIndex,

        .YesCallback = in_rYesCallback,
        .NoCallback = in_rNoCallback,

        .IsOpen = true,
        .Result = -1
    };
}

static void ProcessUpdateMessage(Sonicteam::TitleTask* in_pTitleTask, double in_delayDuration)
{
    static bool isUpdateMessageOpen{};

    if (isUpdateMessageOpen)
        return;

    OpenYesNoMessage("Title_Message_UpdateAvailable", in_delayDuration, 0,
    //
        [&]()
        {
            isUpdateMessageOpen = false;
            UpdateChecker::visitWebsite();
            App::Exit();
        },

        [&]()
        {
            isUpdateMessageOpen = false;
            GuestToHostFunction<int>(sub_82511CA0, in_pTitleTask, 9);
        }
    );

    isUpdateMessageOpen = true;
}

static bool ShouldPlayInOutTransition()
{
    return !Config::DisableTitleInputDelay || g_ignoreDelayPreference;
}

void TitleTask_SetDefaultOption(PPCRegister& r3, PPCRegister& r4)
{
    if (!g_saveDataExists)
        return;
    
    // Set default option to CONTINUE if save data exists.
    reinterpret_cast<Sonicteam::TitleTask*>(g_memory.Translate(r3.u32))->m_SelectedIndex = 1;
    r4.u32 = 5;
}

bool TitleTask_RedirectStateTransitionToOutroAnim(PPCRegister& r31)
{
    if (!g_saveDataExists || !ShouldPlayInOutTransition())
        return false;
    
    // Play Title_Close_02.
    GuestToHostFunction<int>(sub_825119D8, r31.u32, 8, 0);

    return true;
}

// Sonicteam::TitleTask::Update
PPC_FUNC_IMPL(__imp__sub_825126A0);
PPC_FUNC(sub_825126A0)
{
    const auto pTitleTask = reinterpret_cast<Sonicteam::TitleTask*>(base + ctx.r3.u32);
    const auto deltaTime = ctx.f1.f64;

    const auto isUpdateAvailable = UpdateChecker::check() == UpdateChecker::Result::UpdateAvailable;

    switch (pTitleTask->m_State)
    {
        case Sonicteam::TitleTask::TitleState_Open:
        {
            g_ignoreDelayPreference = false;

            // Skip open animation.
            if (Config::DisableTitleInputDelay)
                GuestToHostFunction<int>(sub_82511CA0, pTitleTask, int(Sonicteam::TitleTask::TitleState_Wait));
        }

        case Sonicteam::TitleTask::TitleState_Wait:
        {
            if (OptionsMenu::s_isDebugUnlocked)
                break;

            if (auto& spInputManager = App::s_pApp->m_pDoc->m_InputManagers[0])
            {
                auto& rPadState = spInputManager->PadState;

                if (rPadState.IsPressed(Sonicteam::SoX::Input::PadButton_DPadUp))
                    g_secretFlags = SECRET_UP;

                if ((g_secretFlags & SECRET_UP) != 0 && rPadState.IsPressed(Sonicteam::SoX::Input::PadButton_DPadDown))
                    g_secretFlags |= SECRET_DOWN;

                if ((g_secretFlags & SECRET_DOWN) != 0 && rPadState.IsPressed(Sonicteam::SoX::Input::PadButton_DPadLeft))
                    g_secretFlags |= SECRET_LEFT;

                if ((g_secretFlags & SECRET_LEFT) != 0 && rPadState.IsPressed(Sonicteam::SoX::Input::PadButton_DPadRight))
                    g_secretFlags |= SECRET_RIGHT;
            }

            if (g_secretFlags == SECRET_ALL)
            {
                LOGN_WARNING("Secret unlocked!");
                Game_PlaySound("totalring_count");
                OptionsMenu::s_isDebugUnlocked = true;
            }

            break;
        }

        case Sonicteam::TitleTask::TitleState_OptionsWait:
        {
            if (auto& spInputManager = App::s_pApp->m_pDoc->m_InputManagers[0])
            {
                auto& rPadState = spInputManager->PadState;

                if (g_secretFlags == SECRET_ALL &&
                    rPadState.IsDown(Sonicteam::SoX::Input::PadButton_South) &&
                    rPadState.IsDown(Sonicteam::SoX::Input::PadButton_Start))
                {
                    g_devTitleState = DevTitleState::Open;
                }
            }

            break;
        }

        case Sonicteam::TitleTask::TitleState_OptionsProceed:
        {
            // Reset achievements on new game.
            if (pTitleTask->m_SelectedIndex == 0)
            {
                LOGN("Resetting achievements...");
                AchievementManager::Reset();
            }

            g_ignoreDelayPreference = isUpdateAvailable;
            g_titleProceedOutroTime += deltaTime;

            if (!ShouldPlayInOutTransition())
                g_titleProceedOutroTime = TITLE_OPTION_OUTRO_TOTAL_FRAMES + deltaTime;

            // Wait for outro animation to complete before entering menu.
            if (g_titleProceedOutroTime > TITLE_OPTION_OUTRO_TOTAL_FRAMES)
            {
                if (g_devTitleState == DevTitleState::Open)
                {
                    g_devTitleState = DevTitleState::Start;
                }
                else
                {
                    if (isUpdateAvailable)
                    {
                        ProcessUpdateMessage(pTitleTask, 0.0);
                    }
                    else
                    {
                        GuestToHostFunction<int>(sub_82511CA0, pTitleTask, 9);
                    }
                }
            }

            break;
        }

        case Sonicteam::TitleTask::TitleState_Proceed:
        {
            g_saveDataExists = std::filesystem::exists(GetSaveFilePath(false));

            if (g_saveDataExists)
            {
                // Redirect PRESS START proceed to options open.
                GuestToHostFunction<int>(sub_82511CA0, pTitleTask, int(Sonicteam::TitleTask::TitleState_OptionsOpen));
            }
            else if (isUpdateAvailable)
            {
                ProcessUpdateMessage(pTitleTask, TITLE_OUTRO_FRAMES);
            }

            break;
        }
    }

    if (pTitleTask->m_State != Sonicteam::TitleTask::TitleState_OptionsProceed)
        g_titleProceedOutroTime = 0.0;

    if (ProcessYesNoMessage())
    {
        GuestToHostFunction<int>(sub_82511B10, pTitleTask, deltaTime);

        pTitleTask->m_MovieWaitTime = pTitleTask->m_MovieWaitTime - deltaTime;
        pTitleTask->m_Field60 = 0;

        if (pTitleTask->m_MovieWaitTime < 0.0f)
            pTitleTask->m_MovieWaitTime = 0.0f;
    }
    else
    {
        if (auto& spInputManager = App::s_pApp->m_pDoc->m_InputManagers[0])
        {
            auto& rPadState = spInputManager->PadState;

            if (rPadState.IsPressed(Sonicteam::SoX::Input::PadButton_East))
            {
                if (pTitleTask->m_State == Sonicteam::TitleTask::TitleState_Wait)
                {
                    // Play Title_Close_03.
                    Game_PlaySound("deside");
                    GuestToHostFunction<int>(sub_825119D8, pTitleTask, 9, 0);
                    
                    OpenYesNoMessage("Title_Message_Quit", TITLE_OUTRO_FRAMES, 1, []() { App::Exit(); }, [&]()
                    {
                        // Play Title_Open.
                        pTitleTask->m_MovieWaitTime = Sonicteam::TitleTask::ms_DefaultMovieWaitTime;
                        GuestToHostFunction<int>(sub_825119D8, pTitleTask, 0, 0);
                        g_ignoreDelayPreference = true;
                    });
                }
                else if (pTitleTask->m_State == Sonicteam::TitleTask::TitleState_OptionsWait)
                {
                    // Return to title wait.
                    Game_PlaySound("window_close");
                    GuestToHostFunction<int>(sub_825119D8, pTitleTask, 1, 1);
                    GuestToHostFunction<int>(sub_82511CA0, pTitleTask, (int)Sonicteam::TitleTask::TitleState_Wait);
                }
            }
        }

        __imp__sub_825126A0(ctx, base);
    }
}

PPC_FUNC_IMPL(__imp__sub_82511540);
PPC_FUNC(sub_82511540)
{
    if (!ShouldPlayInOutTransition())
    {
        ctx.r3.u32 = 1;
        return;
    }

    __imp__sub_82511540(ctx, base);
}

// Sonicteam::MainMode::Update
PPC_FUNC_IMPL(__imp__sub_82161A48);
PPC_FUNC(sub_82161A48)
{
    const auto pDoc = reinterpret_cast<Sonicteam::DocMarathon*>(base + ctx.r4.u32);

    if (g_devTitleState == DevTitleState::Start)
    {
        g_devTitleState = DevTitleState::Idle;
        ctx.r3.u32 = g_memory.MapVirtual(pDoc->SetDocState(StateDevTitle::GetInstance()));
        return;
    }

    __imp__sub_82161A48(ctx, base);
}
