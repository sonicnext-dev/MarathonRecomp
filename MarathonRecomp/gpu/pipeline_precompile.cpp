#include "pipeline.h"
#include "guest_shader.h"
#include "render_command.h"
#include "vertex_declaration.h"
#include "video_internal.h"

#include <app.h>
#include <cpu/guest_thread.h>
#include <Marathon.h>
#include <os/thread.h>
#include <patches/hook_event.h>
#include <patches/loading_patches.h>
#include <sdl_listener.h>
#include <shader/shader_cache.h>
#include <user/config.h>
#include <xxHashMap.h>

#if defined(ASYNC_PSO_DEBUG) || defined(PSO_CACHING)
#include <user/paths.h>
#include <magic_enum/magic_enum.hpp>
#endif

#define MARATHON_RECOMP
#include "../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef PSO_CACHING
xxHashMap<PipelineState> g_pipelineStatesToCache;
Mutex g_pipelineCacheMutex;
#endif

std::atomic<uint32_t> g_compilingPipelineTaskCount;
std::atomic<uint32_t> g_pendingPipelineTaskCount;

enum class PipelineTaskType
{
    Null,
    PrecompilePipelines,
    RecompilePipelines
};

struct PipelineTask
{
    PipelineTaskType type{};
};

static Mutex g_pipelineTaskMutex;
static std::vector<PipelineTask> g_pipelineTaskQueue;

class LoadingPipelineCompilationEvent : public ContextHookEvent<Sonicteam::LoadingTask>
{
public:
    void Postfix(Sonicteam::LoadingTask* in_pThis) override
    {
        assert(std::this_thread::get_id() == g_mainThreadId);

        // Wait for pipeline compilations to finish.
        uint32_t value;
        while ((value = g_compilingPipelineTaskCount.load()) != 0)
        {
            // Pump SDL events to prevent the OS
            // from thinking the process is unresponsive.
            SDL_PumpEvents();
            SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);

            g_compilingPipelineTaskCount.wait(value);
        }
    }
}

g_loadingPipelineCompilationEvent{};

static void EnqueuePipelineTask(PipelineTaskType type)
{
    // Precompiled pipelines deliberately do not increment
    // this counter to overlap the compilation with intro logos.
    if (type != PipelineTaskType::PrecompilePipelines)
        ++g_compilingPipelineTaskCount;

    {
        std::lock_guard lock(g_pipelineTaskMutex);
        g_pipelineTaskQueue.emplace_back(type);
    }

    if ((++g_pendingPipelineTaskCount) == 1)
        g_pendingPipelineTaskCount.notify_one();
}

static const PipelineState g_pipelineStateCache[] =
{
#include "cache/pipeline_state_cache.h"
};

#include "cache/vertex_element_cache.h"

static uint8_t* const g_vertexDeclarationCache[] =
{
#include "cache/vertex_declaration_cache.h"
};

// This is passed to pipeline compilation threads to keep the loading screen busy until
// all of them are finished. A shared pointer makes sure the destructor is called only once.
struct PipelineTaskToken
{
    PipelineTaskType type{};

    PipelineTaskToken()
    {
    }

    PipelineTaskToken(const PipelineTaskToken&) = delete;

    PipelineTaskToken(PipelineTaskToken&& other)
        : type(std::exchange(other.type, PipelineTaskType::Null))
    {
    }

    ~PipelineTaskToken()
    {
        if (type != PipelineTaskType::Null)
        {
            if ((--g_compilingPipelineTaskCount) == 0)
                g_compilingPipelineTaskCount.notify_one();
        }
    }
};

struct PipelineStateQueueItem
{
    XXH64_hash_t pipelineHash;
    PipelineState pipelineState;
    std::shared_ptr<PipelineTaskToken> token;
#ifdef ASYNC_PSO_DEBUG
    std::string pipelineName;
#endif
};

static moodycamel::BlockingConcurrentQueue<PipelineStateQueueItem> g_pipelineStateQueue;

static void CompilePipeline(XXH64_hash_t pipelineHash, const PipelineState& pipelineState
#ifdef ASYNC_PSO_DEBUG
    , const std::string& pipelineName
#endif
)
{
    auto pipeline = CreateGraphicsPipeline(pipelineState);
#ifdef ASYNC_PSO_DEBUG
    pipeline->setName(pipelineName);
#endif

    // Will get dropped in render thread if a different thread already managed to compile this.
    g_renderQueue.enqueue(RenderCommands::AddPipeline { .hash = pipelineHash, .pipeline = pipeline.release() });
}

static void PipelineCompilerThread()
{
    os::thread::SetCurrentThreadName("Pipeline Compiler Thread");

#ifdef _WIN32
    int threadPriority = THREAD_PRIORITY_LOWEST;
    SetThreadPriority(GetCurrentThread(), threadPriority);
#endif

    std::unique_ptr<GuestThreadContext> ctx;

    while (true)
    {
        PipelineStateQueueItem queueItem;
        g_pipelineStateQueue.wait_dequeue(queueItem);

        if (ctx == nullptr)
            ctx = std::make_unique<GuestThreadContext>(0);

#ifdef _WIN32
        int newThreadPriority = threadPriority;

        if (App::s_isLoading)
            newThreadPriority = THREAD_PRIORITY_HIGHEST;
        else
            newThreadPriority = THREAD_PRIORITY_LOWEST;

        if (newThreadPriority != threadPriority)
        {
            SetThreadPriority(GetCurrentThread(), newThreadPriority);
            threadPriority = newThreadPriority;
        }
#endif

        CompilePipeline(queueItem.pipelineHash, queueItem.pipelineState
#ifdef ASYNC_PSO_DEBUG
            , queueItem.pipelineName.c_str()
#endif
        );

        std::this_thread::yield();
    }
}

static std::vector<std::unique_ptr<std::thread>> g_pipelineCompilerThreads = []()
    {
        size_t threadCount = std::max(2u, (std::thread::hardware_concurrency() * 2) / 3);

        std::vector<std::unique_ptr<std::thread>> threads(threadCount);
        for (auto& thread : threads)
            thread = std::make_unique<std::thread>(PipelineCompilerThread);

        return threads;
    }();

// Allocate the shared pointer only when new compilations are happening.
// If nothing was compiled, the local "token" variable will get destructed with RAII instead.
struct PipelineTaskTokenPair
{
    PipelineTaskToken token;
    std::shared_ptr<PipelineTaskToken> sharedToken;
};

// Having this separate, because I don't want to lock a mutex in the render thread before
// every single draw. Might be worth profiling to see if it actually has an impact and merge them.
static xxHashMap<PipelineState> g_asyncPipelineStates;

static void EnqueueGraphicsPipelineCompilation(
    const PipelineState& pipelineState,
    PipelineTaskTokenPair& tokenPair,
    const char* name,
    bool isPrecompiledPipeline = false)
{
    XXH64_hash_t hash = GetPipelineStateHash(pipelineState);
    bool shouldCompile = g_asyncPipelineStates.emplace(hash, pipelineState).second;

    if (shouldCompile)
    {
        if (!App::s_isLoading && isPrecompiledPipeline)
        {
            // We can just compile here during the logos.
            CompilePipeline(hash, pipelineState
#ifdef ASYNC_PSO_DEBUG
                , fmt::format("CACHE {} {:X}", name, hash)
#endif
            );
        }
        else
        {
            if (tokenPair.sharedToken == nullptr && tokenPair.token.type != PipelineTaskType::Null)
                tokenPair.sharedToken = std::make_shared<PipelineTaskToken>(std::move(tokenPair.token));

            PipelineStateQueueItem queueItem;
            queueItem.pipelineHash = hash;
            queueItem.pipelineState = pipelineState;
            queueItem.token = tokenPair.sharedToken;
#ifdef ASYNC_PSO_DEBUG
            queueItem.pipelineName = fmt::format("ASYNC {} {:X}", name, hash);
#endif
            g_pipelineStateQueue.enqueue(queueItem);
        }
    }

#ifdef PSO_CACHING_CLEANUP
    if (shouldCompile && isPrecompiledPipeline)
    {
        std::lock_guard lock(g_pipelineCacheMutex);
        g_pipelineStatesToCache.emplace(hash, pipelineState);
    }
#endif

#ifdef PSO_CACHING
    if (!isPrecompiledPipeline)
    {
        std::lock_guard lock(g_pipelineCacheMutex);
        g_pipelineStatesToCache.erase(hash);
    }
#endif
}

static void PipelineTaskConsumerThread()
{
    os::thread::SetCurrentThreadName("Pipeline Task Consumer Thread");

#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
#endif

    std::vector<PipelineTask> localPipelineTaskQueue;
    std::unique_ptr<GuestThreadContext> ctx;

    while (true)
    {
        // Wait for tasks to arrive.
        uint32_t pendingPipelineTaskCount;
        while ((pendingPipelineTaskCount = g_pendingPipelineTaskCount.load()) == 0)
            g_pendingPipelineTaskCount.wait(pendingPipelineTaskCount);

        if (ctx == nullptr)
            ctx = std::make_unique<GuestThreadContext>(0);

        {
            std::lock_guard lock(g_pipelineTaskMutex);
            localPipelineTaskQueue.insert(localPipelineTaskQueue.end(), g_pipelineTaskQueue.begin(), g_pipelineTaskQueue.end());
            g_pipelineTaskQueue.clear();
        }

        bool allHandled = true;

        for (auto& [type] : localPipelineTaskQueue)
        {
            switch (type)
            {
            case PipelineTaskType::PrecompilePipelines:
            {
                // Deliberately leaving the type null to account for the enqueue
                // call not incrementing the compiling pipeline task counter.
                PipelineTaskTokenPair tokenPair;

                for (auto vertexElements : g_vertexDeclarationCache)
                    CreateVertexDeclarationWithoutAddRef(reinterpret_cast<GuestVertexElement*>(vertexElements));

                for (auto pipelineState : g_pipelineStateCache)
                {
                    // The hashes were reinterpret casted to pointers in the cache.
                    pipelineState.vertexShader = GetOrCreateShader(reinterpret_cast<XXH64_hash_t>(pipelineState.vertexShader), ResourceType::VertexShader);

                    if (pipelineState.pixelShader != nullptr)
                        pipelineState.pixelShader = GetOrCreateShader(reinterpret_cast<XXH64_hash_t>(pipelineState.pixelShader), ResourceType::PixelShader);

                    pipelineState.vertexDeclaration = FindVertexDeclaration(reinterpret_cast<XXH64_hash_t>(pipelineState.vertexDeclaration));

                    // Zero out depth bias for Vulkan, we only store common values for D3D12.
                    if (Video::GetRenderDevice()->getCapabilities().dynamicDepthBias && g_backend != Backend::D3D12)
                    {
                        pipelineState.depthBias = 0;
                        pipelineState.slopeScaledDepthBias = 0.0f;
                    }

                    auto createGraphicsPipeline = [&](PipelineState& pipelineStateToCreate, const char* name)
                        {
                            SanitizePipelineState(pipelineStateToCreate);
                            EnqueueGraphicsPipelineCompilation(pipelineStateToCreate, tokenPair, name, true);
                        };

                    // Compile both MSAA and non MSAA variants to work with reflection maps. The render formats are an assumption but it should hold true.
                    if (Config::AntiAliasing != EAntiAliasing::Off &&
                        pipelineState.renderTargetFormat == RenderFormat::R16G16B16A16_FLOAT &&
                        pipelineState.depthStencilFormat == RenderFormat::D32_FLOAT_S8_UINT)
                    {
                        auto msaaPipelineState = pipelineState;
                        msaaPipelineState.sampleCount = int32_t(Config::AntiAliasing.Value);

                        if (Config::TransparencyAntiAliasing && (msaaPipelineState.specConstants & SPEC_CONSTANT_ALPHA_TEST) != 0)
                        {
                            msaaPipelineState.enableAlphaToCoverage = true;
                            msaaPipelineState.specConstants &= ~SPEC_CONSTANT_ALPHA_TEST;
                            msaaPipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TO_COVERAGE;
                        }

                        createGraphicsPipeline(msaaPipelineState, "Precompiled Pipeline MSAA");
                    }

                    createGraphicsPipeline(pipelineState, "Precompiled Pipeline");
                }

                type = PipelineTaskType::Null;
                --g_pendingPipelineTaskCount;

                break;
            }

            case PipelineTaskType::RecompilePipelines:
            {
                PipelineTaskTokenPair tokenPair;
                tokenPair.token.type = type;

                auto asyncPipelines = g_asyncPipelineStates.values();

                for (auto& [hash, pipelineState] : asyncPipelines)
                {
                    bool alphaTest = (pipelineState.specConstants & (SPEC_CONSTANT_ALPHA_TEST | SPEC_CONSTANT_ALPHA_TO_COVERAGE)) != 0;
                    bool msaa = pipelineState.sampleCount != 1 || (pipelineState.renderTargetFormat == RenderFormat::R16G16B16A16_FLOAT && pipelineState.depthStencilFormat == RenderFormat::D32_FLOAT_S8_UINT);

                    pipelineState.sampleCount = 1;
                    pipelineState.enableAlphaToCoverage = false;
                    pipelineState.specConstants &= ~(SPEC_CONSTANT_ALPHA_TEST | SPEC_CONSTANT_ALPHA_TO_COVERAGE);

                    if (msaa && Config::AntiAliasing != EAntiAliasing::Off)
                    {
                        pipelineState.sampleCount = int32_t(Config::AntiAliasing.Value);

                        if (alphaTest)
                        {
                            if (Config::TransparencyAntiAliasing)
                            {
                                pipelineState.enableAlphaToCoverage = true;
                                pipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TO_COVERAGE;
                            }
                            else
                            {
                                pipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TEST;
                            }
                        }
                    }
                    else if (alphaTest)
                    {
                        pipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TEST;
                    }

                    SanitizePipelineState(pipelineState);
                    EnqueueGraphicsPipelineCompilation(pipelineState, tokenPair, "Recompiled Pipeline State");
                }

                type = PipelineTaskType::Null;
                --g_pendingPipelineTaskCount;

                break;
            }
            }
        }

        if (allHandled)
            localPipelineTaskQueue.clear();

        std::this_thread::yield();
    }
}

static std::thread g_pipelineTaskConsumerThread(PipelineTaskConsumerThread);

#ifdef PSO_CACHING
class SDLEventListenerForPSOCaching : public SDLEventListener
{
public:
    bool OnSDLEvent(SDL_Event* event) override
    {
        if (event->type != SDL_EVENT_QUIT)
            return false;

        std::lock_guard lock(g_pipelineCacheMutex);
        if (g_pipelineStatesToCache.empty())
            return false;

        std::string path = (GetUserPath() / "pso_caching.txt").string();
        FILE* f = fopen(path.c_str(), "ab");
        if (f != nullptr)
        {
            ankerl::unordered_dense::set<GuestVertexDeclaration*> vertexDeclarations;
            xxHashMap<PipelineState> pipelineStatesToCache;

            for (auto& [hash, pipelineState] : g_pipelineStatesToCache)
            {
                if (pipelineState.vertexShader->shaderCacheEntry == nullptr ||
                    (pipelineState.pixelShader != nullptr && pipelineState.pixelShader->shaderCacheEntry == nullptr))
                {
                    continue;
                }

                vertexDeclarations.emplace(pipelineState.vertexDeclaration);

                // Mask out the config options.
                pipelineState.sampleCount = 1;
                pipelineState.enableAlphaToCoverage = false;

                if ((pipelineState.specConstants & SPEC_CONSTANT_ALPHA_TO_COVERAGE) != 0)
                {
                    pipelineState.specConstants &= ~SPEC_CONSTANT_ALPHA_TO_COVERAGE;
                    pipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TEST;
                }

                pipelineStatesToCache.emplace(XXH3_64bits(&pipelineState, sizeof(pipelineState)), pipelineState);
            }

            for (auto vertexDeclaration : vertexDeclarations)
            {
                fmt::print(f, "static uint8_t g_vertexElements_{:016X}[] = {{", vertexDeclaration->hash);

                auto bytes = reinterpret_cast<uint8_t*>(vertexDeclaration->vertexElements.get());
                for (size_t i = 0; i < vertexDeclaration->vertexElementCount * sizeof(GuestVertexElement); i++)
                    fmt::print(f, "0x{:X},", bytes[i]);

                fmt::println(f, "}};");
            }

            for (auto& [pipelineHash, pipelineState] : pipelineStatesToCache)
            {
                fmt::println(f, "{{ "
                    "reinterpret_cast<GuestShader*>(0x{:X}),"
                    "reinterpret_cast<GuestShader*>(0x{:X}),"
                    "reinterpret_cast<GuestVertexDeclaration*>(0x{:X}),"
                    "{},"
                    "{},"
                    "{},"
                    "{},"
                    "RenderBlend::{},"
                    "RenderBlend::{},"
                    "RenderCullMode::{},"
                    "RenderFrontFace::{},"
                    "RenderComparisonFunction::{},"
                    "RenderComparisonFunction::{},"
                    "RenderStencilOp::{},"
                    "RenderStencilOp::{},"
                    "RenderStencilOp::{},"
                    "RenderComparisonFunction::{},"
                    "RenderStencilOp::{},"
                    "RenderStencilOp::{},"
                    "RenderStencilOp::{},"
                    "{},"
                    "{},"
                    "{},"
                    "{},"
                    "RenderBlendOperation::{},"
                    "{},"
                    "{},"
                    "RenderBlend::{},"
                    "RenderBlend::{},"
                    "RenderBlendOperation::{},"
                    "0x{:X},"
                    "RenderPrimitiveTopology::{},"
                    "{{ {},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{} }},"
                    "RenderFormat::{},"
                    "RenderFormat::{},"
                    "{},"
                    "{},"
                    "{},"
                    "0x{:X} }},",
                    pipelineState.vertexShader->shaderCacheEntry->hash,
                    pipelineState.pixelShader != nullptr ? pipelineState.pixelShader->shaderCacheEntry->hash : 0,
                    pipelineState.vertexDeclaration->hash,
                    pipelineState.zEnable,
                    pipelineState.zWriteEnable,
                    pipelineState.stencilEnable,
                    pipelineState.stencilTwoSided,
                    magic_enum::enum_name(pipelineState.srcBlend),
                    magic_enum::enum_name(pipelineState.destBlend),
                    magic_enum::enum_name(pipelineState.cullMode),
                    magic_enum::enum_name(pipelineState.frontFace),
                    magic_enum::enum_name(pipelineState.zFunc),
                    magic_enum::enum_name(pipelineState.stencilFunc),
                    magic_enum::enum_name(pipelineState.stencilFail),
                    magic_enum::enum_name(pipelineState.stencilZFail),
                    magic_enum::enum_name(pipelineState.stencilPass),
                    magic_enum::enum_name(pipelineState.stencilFuncCCW),
                    magic_enum::enum_name(pipelineState.stencilFailCCW),
                    magic_enum::enum_name(pipelineState.stencilZFailCCW),
                    magic_enum::enum_name(pipelineState.stencilPassCCW),
                    pipelineState.stencilMask,
                    pipelineState.stencilWriteMask,
                    pipelineState.stencilRef,
                    pipelineState.alphaBlendEnable,
                    magic_enum::enum_name(pipelineState.blendOp),
                    pipelineState.slopeScaledDepthBias,
                    pipelineState.depthBias,
                    magic_enum::enum_name(pipelineState.srcBlendAlpha),
                    magic_enum::enum_name(pipelineState.destBlendAlpha),
                    magic_enum::enum_name(pipelineState.blendOpAlpha),
                    pipelineState.colorWriteEnable,
                    magic_enum::enum_name(pipelineState.primitiveTopology),
                    pipelineState.vertexStrides[0],
                    pipelineState.vertexStrides[1],
                    pipelineState.vertexStrides[2],
                    pipelineState.vertexStrides[3],
                    pipelineState.vertexStrides[4],
                    pipelineState.vertexStrides[5],
                    pipelineState.vertexStrides[6],
                    pipelineState.vertexStrides[7],
                    pipelineState.vertexStrides[8],
                    pipelineState.vertexStrides[9],
                    pipelineState.vertexStrides[10],
                    pipelineState.vertexStrides[11],
                    pipelineState.vertexStrides[12],
                    pipelineState.vertexStrides[13],
                    pipelineState.vertexStrides[14],
                    pipelineState.vertexStrides[15],
                    magic_enum::enum_name(pipelineState.renderTargetFormat),
                    magic_enum::enum_name(pipelineState.depthStencilFormat),
                    pipelineState.sampleCount,
                    pipelineState.enableAlphaToCoverage,
                    pipelineState.enableConditionalSurvey,
                    pipelineState.specConstants);
            }

            fclose(f);
        }

        return false;
    }
};
SDLEventListenerForPSOCaching g_sdlEventListenerForPSOCaching;
#endif

void Video::StartPipelinePrecompilation()
{
    LoadingPatches::Events.push_back(&g_loadingPipelineCompilationEvent);

    EnqueuePipelineTask(PipelineTaskType::PrecompilePipelines);
}

void EnqueuePipelineRecompilation()
{
    EnqueuePipelineTask(PipelineTaskType::RecompilePipelines);
}
