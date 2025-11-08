#include "app.h"
#include <gpu/video.h>
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

static std::thread::id g_mainThreadId = std::this_thread::get_id();

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

    auto pRenderConfig = reinterpret_cast<RenderConfig*>(g_memory.Translate(ctx.r4.u32));
    pRenderConfig->Width = Video::s_viewportWidth;
    pRenderConfig->Height = Video::s_viewportHeight;

    auto pAudioEngine = Sonicteam::AudioEngineXenon::GetInstance();
    pAudioEngine->m_MusicVolume = Config::MusicVolume * Config::MasterVolume;
    pAudioEngine->m_EffectsVolume = Config::EffectsVolume * Config::MasterVolume;

    LOGFN_UTILITY("Changed resolution: {}x{}", pRenderConfig->Width.get(), pRenderConfig->Height.get());

    __imp__sub_8262A568(ctx, base);

    App::s_pApp = (Sonicteam::AppMarathon*)g_memory.Translate(ctx.r3.u32);

    InitPatches();
}

std::array<const char*, 46> g_PauseModeTaskList =
{
    "root",
    "TL_Debug",
    "TL_PrepareMain",
    "TL_Main",
    //"TL_Particle",
    "TL_HeadUpDisplay",
    "TL_PrepareRender",
    "TL_NonStopTask",

    "ResumeWorkerThreadTask",
    "ThreadResumeTask",

    //"Cameras",
    "Cameraman",
    //"DependCameras",
    //"SystemDependCameras",

    "PrepareShadowTask",
    "GameRootTask",
    //"SpkManageTask",
    "Task",
    "TaskAdapter",
    "HUDMainDisplay",
    "PostprocessControllerTask",
    "PrepareRenderTask",
    "PrepareFrameSynchronizerTask",
    "FrameSynchronizerTask",
    "BoundingBoxSynchronizerTask",
    "FrustumCullingTask",
    "UpdateResourceInFrustumTask",
    "GameModeNonStop",

    //AQA
    "ObjectAqaMercurySmall",
    "ObjectAqaPond",
    "ObjectAqaPond", // Duplicate entry
    "ReflectionManager",
    "ReflectionArea",

    //
    "HintWindowTask",
    "HUDMessageWindow"

    //"DocModeExecutor",
    //"GameMode",
    //"DependGame",
    //"PhysicsMaterialMgrRunner",
    //"StageSetManagerRunner",
    //"ActivationMgrRunner",
    // 
    //"ImpactManager",
    //"Entities",
    //"RadarMapIcon",
    //"CTownsKeeper",
    //"CTownsmanActor",
    //"ObjParticle",
    //"CommonPathObj",
    //"BellObj",
    //"WarpGateObj",
    //"TownsDoorObj",
    //"ObjSpring",
    //"ObjectPhysicsSingle",
    //"ObjRadarMapMark",
    //"ObjectPointSample",
    //"StartObj",
    //"Townsman(townsman998)",
    //"CObjBalloonIcon",
    //"ObjWideSpring",
    //"Townsman(townsman021)",
    //"CObjBalloonIcon",
    //"ObjEventCollision",
    //"BellObj",
    //"Townsman(townsman070)",
    //"CObjBalloonIcon",
    //"GondolaObj",
    //"ObjectPointSample",
    //"ObjCameraCollision",
    //"ObjSpring",
    //"Townsman(gondolaman31)",
    //"CTownsmanEquipmentObject",
    //"Townsman(gondolaman29)",
    //"Townsman(gondolaman30)",
    //"ObjSpring",
    //"ObjectPointSample",
    //"TownsDoorObj",
    //"EnemyThread",
    //"Players",
    //"class Sonicteam::Player::Object",
    //"KhronoController",
    //"SceneParamManagerTask",
    //"CommonObjectLensFlare",
    //"KynapseRunnerTask",
    //"KynapseMessenger",
    //"RaderMapManager",
    //"EnemyLifeTask",
    //"MainDisplayTask",
    //"HUDLimitTime",
    //"InputListenTask",
    //"AudioRunner",
    //"PauseTask",
    //"HUDPause"
};


void UpateSpecificTasksPause(Sonicteam::SoX::Engine::Task* ttask)
{
    for (auto& task : *ttask)
    {
        const char* taskName = (&task)->GetName();

        if ((task.m_Flag1 & 4) == 0)
        {

            auto shouldIgnore = std::find_if(g_PauseModeTaskList.begin(), g_PauseModeTaskList.end(),
                [taskName](const char* task) 
                {
                    return strcmp(task ? task : "", taskName) == 0;
                });

            if (shouldIgnore != g_PauseModeTaskList.end())
            {
                (&task)->Update(0.0);
            }

            if (task.m_pDependencies.get())
                UpateSpecificTasksPause(task.m_pDependencies.get());
        }
    }
}

// Sonicteam::DocMarathonState::Update
PPC_FUNC_IMPL(__imp__sub_825EA610);
PPC_FUNC(sub_825EA610)
{
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

    // This function can also be called by the loading thread,
    // which SDL does not like. To prevent the OS from thinking
    // the process is unresponsive, we will flush while waiting
    // for the pipelines to finish compiling in video.cpp.
    if (std::this_thread::get_id() == g_mainThreadId)
    {
        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);

        GameWindow::Update();
    }
    auto pDocState = App::s_pApp->m_pDoc;

    // Allow variable FPS when config is not 60 FPS.
    App::s_pApp->m_pDoc->m_VFrame = Config::FPS != 60;

    AudioPatches::Update(App::s_deltaTime);

    __imp__sub_825EA610(ctx, base);


    //Always Do Specic Tasks during pause, to not 
    if (pDocState->m_PauseFlags.get() != 0)
    {

        GuestToHostFunction<void>(sub_825D49E0, &pDocState->m_CriticalSection1);
        UpateSpecificTasksPause(pDocState->m_pRootTask.get());
        GuestToHostFunction<void>(sub_82580920, &pDocState->m_CriticalSection1);
    }

}

PPC_FUNC_IMPL(__imp__sub_82582648);
PPC_FUNC(sub_82582648)
{
    struct File
    {
    public:
        MARATHON_INSERT_PADDING(4);
        xpointer<const char> pFilePath;
        MARATHON_INSERT_PADDING(0x0C);
        be<uint32_t> Length;
        be<uint32_t> Capacity;
    };

    auto pFile = reinterpret_cast<File*>(base + ctx.r5.u32);

    if (pFile->pFilePath && pFile->Length > 0)
        LOGFN_UTILITY("Loading file: {}", pFile->pFilePath.get());

    __imp__sub_82582648(ctx, base);
}

#if _DEBUG
// Sonicteam::SoX::Thread::Thread
PPC_FUNC_IMPL(__imp__sub_825867A8);
PPC_FUNC(sub_825867A8)
{
    auto pThreadName = (const char*)g_memory.Translate(ctx.r4.u32);

    os::logger::Log(fmt::format("Created thread: {}", pThreadName), os::logger::ELogType::Utility, "Sonicteam::SoX::Thread");

    __imp__sub_825867A8(ctx, base);
}
#endif

PPC_FUNC_IMPL(__imp__sub_82744840);
PPC_FUNC(sub_82744840)
{
    LOG_UTILITY("RenderFrame");

    __imp__sub_82744840(ctx, base);
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

#if _DEBUG
PPC_FUNC_IMPL(__imp__sub_825822D0);
PPC_FUNC(sub_825822D0)
{
    LOG_UTILITY("!!!");

    __imp__sub_825822D0(ctx, base);
}
#endif
