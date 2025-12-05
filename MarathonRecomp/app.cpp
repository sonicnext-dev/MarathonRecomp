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
#include <magic_enum/magic_enum.hpp>

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

// Extend range BEFORE using magic_enum
namespace magic_enum::customize {
    template<>
    struct enum_range<Sonicteam::Spanverse::AckMomServer::MomBase> {
        static constexpr int min = 0;      // Start from 0
        static constexpr int max = 0x10000;   // Or 2000 to be safe
    };
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

    auto pPM = App::s_pApp->GetInstance()->m_pDoc->m_pParticleManager.get();
    if (!pPM || pPM->m_vspManagerUnits.empty()) return;

    auto pAckMgr = static_cast<Sonicteam::Spanverse::AckManager*>(pPM->m_vspManagerUnits[0].get());
    if (!pAckMgr) return;


    printf("pAckMgr %p\n", pAckMgr);

    auto& tags = pAckMgr->m_tlTrunks.m_tlTrunkTags;
    auto current = tags.m_Prev;
    auto size = tags.m_Size.get();

    for (uint32_t i = 0; i < size && current; i++) {
        auto pTag = static_cast<Sonicteam::Spanverse::AckAdminTrunkTag*>(current.get());
        if (!pTag || !pTag->m_pObject) continue;

        auto pObj = pTag->m_pObject.get();
        auto pTrunk = pObj->m_pTrunk.get();
        if (!pTrunk) continue;

        printf("TrunkObj %p Trunk %p\n", pObj, pTrunk);
        for (int y = 0; y < pTrunk->m_pRoot->BranchesCount; y++)
        {
            auto pBranch = pTrunk->m_pRoot->ppBranches[y].get();
            printf("    pBranch[%d] = %p\n", y, pBranch);

            for (int j = 0; j < pBranch->LeafsCount; j++)
            {
                auto pLeaf = pBranch->ppLeafs[j].get();
                printf("        pLeaf[%d] = %p\n", j, pLeaf);
                auto LeafType = magic_enum::enum_name(static_cast<Sonicteam::Spanverse::SpkSpangleServer::SpkSpangleBase>(pLeaf->SpkSpangleBaseType.get()));
                printf("        Type[%d] = %s\n", j, LeafType.data());

                for (int k = 0; k < pLeaf->MomRootCount; k++)
                {
                    auto pMomRoot = pLeaf->MomRoots[k].get();
                    printf("            pMomRoot[%d - %d] = %p\n", k, pMomRoot->Index.get(),pMomRoot);

                    for (int f = 0; f < pMomRoot->Count; f++)
                    {
                        auto pAcmMomNode = pMomRoot->Children[f].get();

                        printf("                pAcmMomNode[%d - %s] = %p\n",
                            f,
                            magic_enum::enum_name(static_cast<Sonicteam::Spanverse::AckMomServer::MomBase>(pAcmMomNode->MomBaseID.get())).data(),
                            pAcmMomNode); 
                    }
                   

                }
            }
        }


   

        current = current->m_pNext;
    }
    printf("--------------------------------------------------------------\n");

    /*
    auto shaderMGR = Sonicteam::SoX::Graphics::ShaderMgr::GetInstance();
    if (manager->m_mResources.find(shaderMGR->m_MgrIndex) != manager->m_mResources.end())
    {
        auto& resources = manager->m_mResources[shaderMGR->m_MgrIndex];
        printf("[\n");

        for (auto it = resources.begin(); it != resources.end(); ++it)
        {
            auto& shader = *it;
            auto RTTI = stdx::VftableToRTTI(shader.second->m_pVftable.get());
            auto pShader = static_cast<Sonicteam::SoX::Graphics::ShaderFXL*>(shader.second.get());

            printf("  {\n");
            printf("    \"name\": \"%s\",\n", shader.first.c_str());
            printf("    \"type\": \"%s\",\n", RTTI->typeDesc->name().c_str());
            printf("    \"vftable\": \"%p\",\n", shader.second->m_pVftable.get());
            printf("    \"pointer\": \"%p\",\n", shader.second.get());
            printf("    \"m_FXLName\": \"%s\",\n", pShader->m_FXLName.c_str());

            // Set the FXL name
            pShader->m_FXLName = "CharacterFX";

            printf("    \"m_mPassFXL\": [\n");

            bool firstPass = true;
            for (auto testIt = pShader->m_mvPassesFXL.begin(); testIt != pShader->m_mvPassesFXL.end(); ++testIt)
            {
                // Print the key and vector info
                if (!firstPass) {
                    printf(",\n");
                }
                printf("      {\n");
                printf("        \"key\": \"0x%x\",\n", testIt->first.get());
                printf("        \"passes\": [\n");

                // Print each pass in the vector
                bool firstVector = true;
                for (auto& test2T : testIt->second)
                {
                    if (!firstVector) {
                        printf(",\n");
                    }
                    auto passRTTI = stdx::VftableToRTTI(test2T.m_pVftable.get());
                    printf("          {\n");
                    printf("            \"pass_vftable\": \"%p\",\n", test2T.m_pVftable.get());
                    printf("            \"pass_pointer\": \"%p\",\n", &test2T);
                    printf("            \"pass_type\": \"%s\"\n", passRTTI->typeDesc->name().c_str());
                    printf("          }");
                    firstVector = false;
                }

                printf("\n        ]\n");
                printf("      }");
                firstPass = false;
            }

            printf("\n    ]\n");
            printf("  }%s\n", ",");
        }

        printf("]\n");
    }
 
 */
    // Allow variable FPS when config is not 60 FPS.
    App::s_pApp->m_pDoc->m_VFrame = Config::FPS != 60;

    AudioPatches::Update(App::s_deltaTime);

    __imp__sub_825EA610(ctx, base);
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
