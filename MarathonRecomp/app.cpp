#include "app.h"
#include <gpu/imgui/imgui_snapshot.h>
#include <gpu/video.h>
#include <hid/message/messages.h>
#include <hid/hid.h>
#include <install/installer.h>
#include <kernel/function.h>
#include <os/process.h>
#include <os/logger.h>
#include <patches/audio_patches.h>
#include <patches/patches.h>
#include <ui/game_window.h>
#include <user/config.h>
#include <user/paths.h>
#include <user/registry.h>
#include <utils/host_thread.h>
#include <xxHashMap.h>

static uint32_t g_mainThreadId = HostThread::GetCurrentID();
extern std::array<XXH64_hash_t, 29> g_taskUpdatesOnPause;

void App::Update(double in_deltaTime)
{
    if (HostThread::GetCurrentID() == g_mainThreadId)
    {
        SDL_PumpEvents();
        SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);

        GameWindow::Update();
    }

    AudioPatches::Update(in_deltaTime);
}

void App::Restart(std::vector<std::string> restartArgs)
{
    os::process::StartProcess(os::process::GetExecutablePath(), restartArgs, os::process::GetWorkingDirectory());
    Exit();
}

void App::Exit()
{
    Config::Save();

#ifdef _WIN32
    timeEndPeriod(1);
#endif

    std::_Exit(0);
}

// Sonicteam::AppMarathon::AppMarathon
PPC_FUNC_IMPL(__imp__sub_8262A568);
PPC_FUNC(sub_8262A568)
{
    App::s_isInit = true;
    App::s_isMissingDLC = true;
    App::s_language = Config::Language;

    Sonicteam::Globals::Init();
    Registry::Save();

    struct RenderConfig
    {
        be<uint32_t> Width;
        be<uint32_t> Height;
    };

    auto pRenderConfig = reinterpret_cast<RenderConfig*>(base + ctx.r4.u32);
    pRenderConfig->Width = Video::s_viewportWidth;
    pRenderConfig->Height = Video::s_viewportHeight;

    auto pAudioEngine = Sonicteam::AudioEngineXenon::GetInstance();
    pAudioEngine->m_MusicVolume = Config::MusicVolume * Config::MasterVolume;
    pAudioEngine->m_EffectsVolume = Config::EffectsVolume * Config::MasterVolume;

    LOGFN_UTILITY("Changed resolution: {}x{}", pRenderConfig->Width.get(), pRenderConfig->Height.get());

    Video::StartPipelinePrecompilation();

    __imp__sub_8262A568(ctx, base);

    App::s_pApp = reinterpret_cast<Sonicteam::AppMarathon*>(base + ctx.r3.u32);

    ImFontAtlasSnapshot::Init();
    InitPatches();
}

// Sonicteam::DocMarathonState::Update
PPC_FUNC_IMPL(__imp__sub_825EA610);
PPC_FUNC(sub_825EA610)
{
    const auto pDoc = reinterpret_cast<Sonicteam::DocMarathonState*>(base + ctx.r3.u32);

    Video::WaitOnSwapChain();

    // Correct small delta time errors.
    if (Config::FPS >= FPS_MIN && Config::FPS < FPS_MAX)
    {
        double targetDeltaTime = 1.0 / Config::FPS;

        if (abs(ctx.f1.f64 - targetDeltaTime) < 0.00001)
            ctx.f1.f64 = targetDeltaTime;
    }

    App::s_deltaTime = ctx.f1.f64;
    App::s_time += App::s_deltaTime;
    App::Update(App::s_deltaTime);

    __imp__sub_825EA610(ctx, base);

    App::s_isPaused = pDoc->m_PauseFlags.get();

    if (App::s_isPaused)
    {
        // Update visual tasks for resizing buffers on pause.
        pDoc->m_pRootTask->Walk([](Sonicteam::SoX::Engine::Task* in_pTask) -> bool
        {
            if ((in_pTask->m_Flags & 4) != 0)
                return true;

            const auto taskNameHash = HashStr(in_pTask->GetName());

            if (std::find(g_taskUpdatesOnPause.begin(), g_taskUpdatesOnPause.end(), taskNameHash) != g_taskUpdatesOnPause.end())
                in_pTask->Update();

            return true;
        });
    }

    const auto pHidManager = HID::GetManager();

    if (const auto pGameMode = pDoc->GetDocMode<Sonicteam::GameMode>("GameMode"))
    {
        switch (pGameMode->GetGame()->m_State)
        {
            case Sonicteam::GameImp::GameState_Stage:
            case Sonicteam::GameImp::GameState_Quit:
                pHidManager->BroadcastMessage<HID::MsgSetMouseCapture>(!App::s_isPaused);
                break;

            default:
                pHidManager->BroadcastMessage<HID::MsgSetMouseCapture>(false);
                break;
        }
    }
    else
    {
        pHidManager->BroadcastMessage<HID::MsgSetMouseCapture>(false);
    }
}

// Sonicteam::SpanverseHeap::Alloc
PPC_FUNC_IMPL(__imp__sub_825E7918);
PPC_FUNC(sub_825E7918)
{
#if _DEBUG
    os::logger::Log(fmt::format("Allocated {} bytes", ctx.r3.u32), os::logger::ELogType::Utility, "Sonicteam::SpanverseHeap");
#endif

    // This function checks if R4 is non-zero
    // to allow an allocation, but it's always
    // passed in as zero.
    ctx.r4.u32 = 1;

    __imp__sub_825E7918(ctx, base);
}

// Sonicteam::SpanverseHeap::Free
PPC_FUNC_IMPL(__imp__sub_825E7958);
PPC_FUNC(sub_825E7958)
{
#if _DEBUG
    os::logger::Log(fmt::format("Freed {:08X}", ctx.r3.u32), os::logger::ELogType::Utility, "Sonicteam::SpanverseHeap");
#endif

    // This function checks if R4 is non-zero
    // to allow a free, but it's always
    // passed in as zero.
    ctx.r4.u32 = 1;

    __imp__sub_825E7958(ctx, base);
}

std::array<XXH64_hash_t, 29> g_taskUpdatesOnPause =
{
    HashStr("root"),
    HashStr("TL_Debug"),
    HashStr("TL_PrepareMain"),
    HashStr("TL_Main"),
    HashStr("TL_HeadUpDisplay"),
    HashStr("TL_PrepareRender"),
    HashStr("TL_NonStopTask"),
    HashStr("ResumeWorkerThreadTask"),
    HashStr("ThreadResumeTask"),
    HashStr("Cameraman"),
    HashStr("PrepareShadowTask"),
    HashStr("GameRootTask"),
    HashStr("Task"),
    HashStr("TaskAdapter"),
    HashStr("HUDMainDisplay"),
    HashStr("PostprocessControllerTask"),
    HashStr("PrepareRenderTask"),
    HashStr("PrepareFrameSynchronizerTask"),
    HashStr("FrameSynchronizerTask"),
    HashStr("BoundingBoxSynchronizerTask"),
    HashStr("FrustumCullingTask"),
    HashStr("UpdateResourceInFrustumTask"),
    HashStr("GameModeNonStop"),
    HashStr("ObjectAqaMercurySmall"),
    HashStr("ObjectAqaPond"),
    HashStr("ReflectionManager"),
    HashStr("ReflectionArea"),
    HashStr("HintWindowTask"),
    HashStr("HUDMessageWindow")
};
