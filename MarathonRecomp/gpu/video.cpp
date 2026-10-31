#include "video.h"

#include "guest_convert.h"
#include "guest_device.h"
#include "surface_variant.h"
#include "imgui/imgui_common.h"
#include "imgui/imgui_snapshot.h"
#include "render_command.h"
#include "resources.h"
#include "video_internal.h"

#include <app.h>
#include <cpu/guest_thread.h>
#include <cstdint>
#include <kernel/function.h>
#include <kernel/heap.h>
#include <hid/hid.h>
#include <kernel/memory.h>
#include <kernel/xdbf.h>
#include <plume_render_interface.h>
#include <shader/shader_cache.h>
#include <Marathon.h>
#include <ui/achievement_menu.h>
#include <ui/achievement_overlay.h>
#include <ui/button_window.h>
#include <ui/fader.h>
#include <ui/imgui_utils.h>
#include <ui/installer_wizard.h>
#include <ui/message_window.h>
#include <ui/options_menu.h>
#include <ui/game_window.h>
#include <ui/black_bar.h>
#include <patches/aspect_ratio_patches.h>
#include <patches/hook_event.h>
#include <patches/loading_patches.h>
#include <user/config.h>
#include <sdl_listener.h>
#include <xxHashMap.h>
#include <os/process.h>
#include <os/thread.h>

#if defined(ASYNC_PSO_DEBUG) || defined(PSO_CACHING)
#include <user/paths.h>
#include <magic_enum/magic_enum.hpp>
#endif

#define MARATHON_RECOMP
#include "../../tools/XenosRecomp/XenosRecomp/shader_common.h"
#include "gpu/shader/shaders.h"

#ifdef _WIN32
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace plume
{
#ifdef MARATHON_RECOMP_D3D12
    extern std::unique_ptr<RenderInterface> CreateD3D12Interface();
#endif
#ifdef MARATHON_RECOMP_METAL
extern std::unique_ptr<RenderInterface> CreateMetalInterface();
#endif
    extern std::unique_ptr<RenderInterface> CreateVulkanInterface();
}

using namespace plume;

#pragma pack(push, 1)
struct PipelineState
{
    GuestShader* vertexShader = nullptr;
    GuestShader* pixelShader = nullptr;
    GuestVertexDeclaration* vertexDeclaration = nullptr;
    bool zEnable = true;
    bool zWriteEnable = true;
    bool stencilEnable = false;
    bool stencilTwoSided = false;
    RenderBlend srcBlend = RenderBlend::ONE;
    RenderBlend destBlend = RenderBlend::ZERO;
    RenderCullMode cullMode = RenderCullMode::NONE;
    RenderFrontFace frontFace = RenderFrontFace::CLOCKWISE;
    RenderComparisonFunction zFunc = RenderComparisonFunction::LESS;
    RenderComparisonFunction stencilFunc = RenderComparisonFunction::ALWAYS;
    RenderStencilOp stencilFail = RenderStencilOp::KEEP;
    RenderStencilOp stencilZFail = RenderStencilOp::KEEP;
    RenderStencilOp stencilPass = RenderStencilOp::KEEP;
    RenderComparisonFunction stencilFuncCCW = RenderComparisonFunction::ALWAYS;
    RenderStencilOp stencilFailCCW = RenderStencilOp::KEEP;
    RenderStencilOp stencilZFailCCW = RenderStencilOp::KEEP;
    RenderStencilOp stencilPassCCW = RenderStencilOp::KEEP;
    uint32_t stencilMask = 0xFFFFFFFF;
    uint32_t stencilWriteMask = 0xFFFFFFFF;
    uint32_t stencilRef = 0;
    bool alphaBlendEnable = false;
    RenderBlendOperation blendOp = RenderBlendOperation::ADD;
    float slopeScaledDepthBias = 0.0f;
    int32_t depthBias = 0;
    RenderBlend srcBlendAlpha = RenderBlend::ONE;
    RenderBlend destBlendAlpha = RenderBlend::ZERO;
    RenderBlendOperation blendOpAlpha = RenderBlendOperation::ADD;
    uint32_t colorWriteEnable = uint32_t(RenderColorWriteEnable::ALL);
    RenderPrimitiveTopology primitiveTopology = RenderPrimitiveTopology::TRIANGLE_LIST;
    uint8_t vertexStrides[16]{};
    RenderFormat renderTargetFormat{};
    RenderFormat depthStencilFormat{};
    RenderSampleCounts sampleCount = RenderSampleCount::COUNT_1;
    bool enableAlphaToCoverage = false;
    bool enableConditionalSurvey = false;
    uint32_t specConstants = 0;
};
#pragma pack(pop)

struct UploadAllocation
{
    const RenderBuffer* buffer;
    uint64_t offset;
    uint8_t* memory;
    uint64_t deviceAddress;
};

struct SharedConstants
{
    uint32_t texture2DIndices[16]{};
    uint32_t texture2DArrayIndices[16]{};
    uint32_t textureCubeIndices[16]{};
    uint32_t samplerIndices[16]{};
    uint32_t booleans{};
    uint32_t swappedTexcoords{};
    uint32_t swappedNormals{};
    uint32_t swappedBinormals{};
    uint32_t swappedTangents{};
    uint32_t swappedBlendWeights{};
    float halfPixelOffsetX{};
    float halfPixelOffsetY{};
    float clipPlane[4]{};
    bool clipPlaneEnabled{};
    float alphaThreshold{};
    uint32_t conditionalSurveyIndex{};
    uint32_t conditionalRenderingIndex{};
};

// Depth bias values here are only used when the render device has 
// dynamic depth bias capability enabled. Otherwise, they get unused
// and the values get assigned in the pipeline state instead.

static GuestSurface* g_renderTarget;
static GuestSurface* g_depthStencil;
static RenderFramebuffer* g_framebuffer;
static RenderViewport g_viewport(0.0f, 0.0f, 1280.0f, 720.0f);
static PipelineState g_pipelineState;
static int32_t g_depthBias;
static float g_slopeScaledDepthBias;
static uint32_t g_vertexShaderConstants[0x400];
static uint32_t g_pixelShaderConstants[0x380];
static SharedConstants g_sharedConstants;
static GuestTexture* g_textures[16];
static RenderSamplerDesc g_samplerDescs[16];
static bool g_scissorTestEnable = false;
static RenderRect g_scissorRect;
static RenderVertexBufferView g_vertexBufferViews[16];
static RenderInputSlot g_inputSlots[16];
static RenderIndexBufferView g_indexBufferView({}, 0, RenderFormat::R16_UINT);

struct DirtyStates
{
    bool renderTargetAndDepthStencil;
    bool viewport;
    bool pipelineState;
    bool depthBias;
    bool sharedConstants;
    bool scissorRect;
    bool vertexShaderConstants;
    uint8_t vertexStreamFirst;
    uint8_t vertexStreamLast;
    bool indices;
    bool pixelShaderConstants;

    DirtyStates(bool value)
        : renderTargetAndDepthStencil(value)
        , viewport(value)
        , pipelineState(value)
        , depthBias(value)
        , sharedConstants(value)
        , scissorRect(value)
        , vertexShaderConstants(value)
        , vertexStreamFirst(value ? 0 : 255)
        , vertexStreamLast(value ? 15 : 0)
        , indices(value)
        , pixelShaderConstants(value)
    {
    }
};

static DirtyStates g_dirtyStates(true);

template<typename T>
static void SetDirtyValue(bool& dirtyState, T& dest, const T& src)
{
    if (dest != src)
    {
        dest = src;
        dirtyState = true;
    }
}

static constexpr size_t PROFILER_VALUE_COUNT = 256;
static size_t g_profilerValueIndex;

struct Profiler
{
    std::atomic<double> value;
    double values[PROFILER_VALUE_COUNT];
    std::chrono::steady_clock::time_point start;

    void Begin()
    {
        start = std::chrono::steady_clock::now();
    }

    void End()
    {
        value = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
    }

    void Set(double v)
    {
        value = v;
    }

    void Reset()
    {
        End();
        Begin();
    }

    double UpdateAndReturnAverage()
    {
        values[g_profilerValueIndex] = value;
        return std::accumulate(values, values + PROFILER_VALUE_COUNT, 0.0) / PROFILER_VALUE_COUNT;
    }
};

static double g_applicationValues[PROFILER_VALUE_COUNT];
static Profiler g_gpuFrameProfiler;
static Profiler g_presentProfiler;
static Profiler g_frameFenceProfiler;
static Profiler g_presentWaitProfiler;
static Profiler g_swapChainAcquireProfiler;

static bool g_profilerVisible;
static bool g_profilerWasToggled;

#if !defined(MARATHON_RECOMP_D3D12) && !defined(MARATHON_RECOMP_METAL)
static constexpr Backend g_backend = Backend::VULKAN;
#else
static Backend g_backend;
#endif

static bool g_triangleStripWorkaround = false;

static std::unique_ptr<RenderInterface> g_interface;
static std::unique_ptr<RenderDevice> g_device;

static RenderDeviceCapabilities g_capabilities;

static constexpr size_t NUM_FRAMES = 2;
static constexpr size_t NUM_QUERIES = 2;

static uint32_t g_frame = 0;
static uint32_t g_nextFrame = 1;
static uint32_t g_renderFrame = 0;

static std::unique_ptr<RenderCommandQueue> g_queue;
static std::unique_ptr<RenderCommandList> g_commandLists[NUM_FRAMES];
static std::unique_ptr<RenderCommandFence> g_commandFences[NUM_FRAMES];
static std::unique_ptr<RenderQueryPool> g_queryPools[NUM_FRAMES];
static bool g_commandListStates[NUM_FRAMES];

static Mutex g_copyMutex;
static std::unique_ptr<RenderCommandQueue> g_copyQueue;
static std::unique_ptr<RenderCommandList> g_copyCommandList;
static std::unique_ptr<RenderCommandFence> g_copyCommandFence;

static Mutex g_discardMutex;
static std::unique_ptr<RenderCommandList> g_discardCommandList;
static std::unique_ptr<RenderCommandFence> g_discardCommandFence;

static std::unique_ptr<RenderSwapChain> g_swapChain;
static bool g_swapChainValid;

static constexpr RenderFormat BACKBUFFER_FORMAT = RenderFormat::B8G8R8A8_UNORM;

static std::unique_ptr<RenderCommandSemaphore> g_acquireSemaphores[NUM_FRAMES];
static std::vector<std::unique_ptr<RenderCommandSemaphore>> g_renderSemaphores;
static uint32_t g_backBufferIndex;
static std::unique_ptr<GuestSurface> g_backBufferHolder;
static GuestSurface* g_backBuffer;

static std::unique_ptr<RenderTexture> g_intermediaryBackBufferTexture;
static uint32_t g_intermediaryBackBufferTextureWidth;
static uint32_t g_intermediaryBackBufferTextureHeight;
static uint32_t g_intermediaryBackBufferTextureDescriptorIndex;

static std::unique_ptr<RenderPipeline> g_gammaCorrectionPipeline;

std::unique_ptr<RenderDescriptorSet> g_textureDescriptorSet;
static std::unique_ptr<RenderDescriptorSet> g_samplerDescriptorSet;

static std::unique_ptr<RenderBuffer> g_conditionalSurveyBuffer;
static std::unique_ptr<RenderDescriptorSet> g_conditionalSurveyDescriptorSet;

static std::unique_ptr<RenderTexture> g_blankTextures[TEXTURE_DESCRIPTOR_NULL_COUNT];
static std::unique_ptr<RenderTextureView> g_blankTextureViews[TEXTURE_DESCRIPTOR_NULL_COUNT];

TextureDescriptorAllocator g_textureDescriptorAllocator;

static std::unique_ptr<RenderPipelineLayout> g_pipelineLayout;
static xxHashMap<std::unique_ptr<RenderPipeline>> g_pipelines;

#ifdef ASYNC_PSO_DEBUG
static std::atomic<uint32_t> g_pipelinesCreatedInRenderThread;
static std::atomic<uint32_t> g_pipelinesCreatedAsynchronously;
static std::atomic<uint32_t> g_pipelinesDropped;
static std::atomic<uint32_t> g_pipelinesCurrentlyCompiling;
static std::string g_pipelineDebugText;
static Mutex g_debugMutex;
#endif

#ifdef PSO_CACHING
static xxHashMap<PipelineState> g_pipelineStatesToCache;
static Mutex g_pipelineCacheMutex;
#endif

static std::atomic<uint32_t> g_compilingPipelineTaskCount;
static std::atomic<uint32_t> g_pendingPipelineTaskCount;

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

std::thread::id g_mainThreadId = std::this_thread::get_id();

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

static xxHashMap<std::pair<uint32_t, std::unique_ptr<RenderSampler>>> g_samplerStates;

static Mutex g_vertexDeclarationMutex;
static xxHashMap<GuestVertexDeclaration*> g_vertexDeclarations;

struct UploadBuffer
{
    static constexpr size_t SIZE = 16 * 1024 * 1024;

    std::unique_ptr<RenderBuffer> buffer;
    uint8_t* memory = nullptr;
    uint64_t deviceAddress = 0;
};

struct UploadAllocator
{
    std::vector<UploadBuffer> buffers;
    uint32_t index = 0;
    uint32_t offset = 0;

    UploadAllocation allocate(uint32_t size, uint32_t alignment)
    {
        assert(size <= UploadBuffer::SIZE);

        offset = (offset + alignment - 1) & ~(alignment - 1);

        if (offset + size > UploadBuffer::SIZE)
        {
            ++index;
            offset = 0;
        }

        if (buffers.size() <= index)
            buffers.resize(index + 1);

        auto& buffer = buffers[index];
        if (buffer.buffer == nullptr)
        {
            buffer.buffer = g_device->createBuffer(RenderBufferDesc::UploadBuffer(UploadBuffer::SIZE, RenderBufferFlag::CONSTANT | RenderBufferFlag::VERTEX | RenderBufferFlag::INDEX | RenderBufferFlag::DEVICE_ADDRESSABLE));
            buffer.memory = reinterpret_cast<uint8_t*>(buffer.buffer->map());
            buffer.deviceAddress = buffer.buffer->getDeviceAddress();
        }
        
        auto ref = buffer.buffer->at(offset);
        offset += size;

        return { ref.ref, ref.offset, buffer.memory + ref.offset, buffer.deviceAddress + ref.offset };
    }

    template<bool TByteSwap, typename T>
    UploadAllocation allocate(const T* memory, uint32_t size, uint32_t alignment)
    {
        auto result = allocate(size, alignment);

        if constexpr (TByteSwap)
        {
            auto destination = reinterpret_cast<T*>(result.memory);

            for (size_t i = 0; i < size; i += sizeof(T))
            {
                *destination = ByteSwap(*memory);
                ++destination;
                ++memory;
            }
        }
        else
        {
            memcpy(result.memory, memory, size);
        }

        return result;
    }

    void reset()
    {
        index = 0;
        offset = 0;
    }
};

static UploadAllocator g_uploadAllocators[NUM_FRAMES];

static std::vector<GuestResource*> g_tempResources[NUM_FRAMES];
static std::vector<std::unique_ptr<RenderBuffer>> g_tempBuffers[NUM_FRAMES];

template<GuestPrimitiveType PrimitiveType>
struct PrimitiveIndexData
{
    std::vector<uint16_t> indexData;
    RenderBufferReference indexBuffer;
    uint32_t currentIndexCount = 0;

    uint32_t prepare(uint32_t guestPrimCount)
    {
        uint32_t primCount;
        uint32_t indexCountPerPrimitive;

        switch (PrimitiveType)
        {
        case D3DPT_TRIANGLEFAN:
            primCount = guestPrimCount - 2;
            indexCountPerPrimitive = 3; 
            break;
        case D3DPT_QUADLIST:
            primCount = guestPrimCount / 4;
            indexCountPerPrimitive = 6;
            break;
        default:
            assert(false && "Unknown primitive type.");
            break;
        }

        uint32_t indexCount = primCount * indexCountPerPrimitive;

        if (indexData.size() < indexCount)
        {
            const size_t oldPrimCount = indexData.size() / indexCountPerPrimitive;
            indexData.resize(indexCount);

            for (size_t i = oldPrimCount; i < primCount; i++)
            {
                switch (PrimitiveType)
                {
                case D3DPT_TRIANGLEFAN:
                {
                    indexData[i * 3 + 0] = 0;
                    indexData[i * 3 + 1] = static_cast<uint16_t>(i + 1);
                    indexData[i * 3 + 2] = static_cast<uint16_t>(i + 2);
                    break;
                }
                case D3DPT_QUADLIST:
                {
                    indexData[i * 6 + 0] = static_cast<uint16_t>(i * 4 + 0);
                    indexData[i * 6 + 1] = static_cast<uint16_t>(i * 4 + 1);
                    indexData[i * 6 + 2] = static_cast<uint16_t>(i * 4 + 2);

                    indexData[i * 6 + 3] = static_cast<uint16_t>(i * 4 + 0);
                    indexData[i * 6 + 4] = static_cast<uint16_t>(i * 4 + 2);
                    indexData[i * 6 + 5] = static_cast<uint16_t>(i * 4 + 3);
                    break;
                }
                default:
                    assert(false && "Unknown primitive type.");
                    break;
                }
            }
        }

        if (indexBuffer == NULL || currentIndexCount < indexCount)
        {
            auto allocation = g_uploadAllocators[g_frame].allocate<false>(indexData.data(), indexCount * 2, 2);
            indexBuffer = allocation.buffer->at(allocation.offset);
            currentIndexCount = indexCount;
        }

        SetDirtyValue(g_dirtyStates.indices, g_indexBufferView.buffer, indexBuffer);
        SetDirtyValue(g_dirtyStates.indices, g_indexBufferView.size, indexCount * 2);
        SetDirtyValue(g_dirtyStates.indices, g_indexBufferView.format, RenderFormat::R16_UINT);

        return indexCount;
    }

    void reset()
    {
        indexBuffer = {};
        currentIndexCount = 0;
    }
};

static PrimitiveIndexData<D3DPT_TRIANGLEFAN> g_triangleFanIndexData;
static PrimitiveIndexData<D3DPT_QUADLIST> g_quadIndexData;

static ankerl::unordered_dense::set<GuestSurface*> g_pendingSurfaceCopies;
static ankerl::unordered_dense::set<GuestSurface*> g_pendingResolves;

static void DestructTempResources()
{
    for (auto resource : g_tempResources[g_frame])
    {
        switch (resource->type)
        {
        case ResourceType::Texture:
        case ResourceType::VolumeTexture:
        case ResourceType::ArrayTexture:
        {
            const auto texture = reinterpret_cast<GuestTexture*>(resource);

            if (texture->sourceSurface != nullptr)
                texture->sourceSurface->destinationTextures.erase(texture);

            if (texture->mappedMemory != nullptr) {
                g_userHeap.Free(texture->mappedMemory);
            }

            g_textureDescriptorSet->setTexture(texture->descriptorIndex, nullptr, {});
            g_textureDescriptorAllocator.free(texture->descriptorIndex);

            if (texture->patchedTexture != nullptr)
            {
                g_textureDescriptorSet->setTexture(texture->patchedTexture->descriptorIndex, nullptr, {});
                g_textureDescriptorAllocator.free(texture->patchedTexture->descriptorIndex);
            }

            texture->~GuestTexture();
            break;
        }

        case ResourceType::VertexBuffer:
        case ResourceType::IndexBuffer:
        {
            const auto buffer = reinterpret_cast<GuestBuffer*>(resource);

            if (buffer->mappedMemory != nullptr)
                g_userHeap.Free(buffer->mappedMemory);

            buffer->~GuestBuffer();
            break;
        }

        case ResourceType::RenderTarget:
        case ResourceType::DepthStencil:
        {
            const auto surface = reinterpret_cast<GuestSurface*>(resource);

            g_pendingSurfaceCopies.erase(surface);
            g_pendingResolves.erase(surface);

            if (g_renderTarget == surface)
            {
                g_renderTarget = nullptr;
                g_dirtyStates.renderTargetAndDepthStencil = true;
            }

            if (g_depthStencil == surface)
            {
                g_depthStencil = nullptr;
                g_dirtyStates.renderTargetAndDepthStencil = true;
            }

            for (const auto& [texture, _] : surface->destinationTextures)
                texture->sourceSurface = nullptr;

            if (surface->variant != nullptr)
            {
                if (ReleaseSurfaceVariant(surface->variant))
                    g_framebuffer = nullptr;
            }
            else if (surface->descriptorIndex != NULL)
            {
                g_textureDescriptorSet->setTexture(surface->descriptorIndex, nullptr, {});
                g_textureDescriptorAllocator.free(surface->descriptorIndex);
            }

            surface->~GuestSurface();
            break;
        }

        case ResourceType::VertexDeclaration:
            reinterpret_cast<GuestVertexDeclaration*>(resource)->~GuestVertexDeclaration();
            break;

        case ResourceType::VertexShader:
        case ResourceType::PixelShader:
        {
            reinterpret_cast<GuestShader*>(resource)->~GuestShader();
            break;
        }
        }

        g_userHeap.Free(resource);
    }

    g_tempResources[g_frame].clear();
    g_tempBuffers[g_frame].clear();
}

static ankerl::unordered_dense::map<RenderTexture*, RenderTextureLayout> g_barrierMap;

static RenderTextureLayout& GetLayoutRef(GuestBaseTexture* texture)
{
    if (texture->type == ResourceType::RenderTarget || texture->type == ResourceType::DepthStencil)
    {
        const auto surface = reinterpret_cast<GuestSurface*>(texture);
        if (surface->variant != nullptr && surface->texture == surface->variant->textureHolder.get())
            return surface->variant->layout;
    }

    return texture->layout;
}

static void AddBarrier(GuestBaseTexture* texture, RenderTextureLayout layout)
{
    if (texture != nullptr)
    {
        auto& layoutRef = GetLayoutRef(texture);
        if (layoutRef != layout)
        {
            g_barrierMap[texture->texture] = layout;
            layoutRef = layout;
        }
    }
}

static std::vector<RenderTextureBarrier> g_barriers;

static void FlushBarriers()
{
    if (!g_barrierMap.empty())
    {
        for (auto& [texture, layout] : g_barrierMap)
            g_barriers.emplace_back(texture, layout);

        g_commandLists[g_frame]->barriers(RenderBarrierStage::GRAPHICS | RenderBarrierStage::COPY, g_barriers);

        g_barrierMap.clear();
        g_barriers.clear();
    }
}

static std::unique_ptr<uint8_t[]> g_shaderCache;

static void LoadEmbeddedResources()
{
    switch (g_backend)
    {
    case Backend::VULKAN:
        g_shaderCache = std::make_unique<uint8_t[]>(g_spirvCacheDecompressedSize);
        ZSTD_decompress(g_shaderCache.get(), g_spirvCacheDecompressedSize, g_compressedSpirvCache, g_spirvCacheCompressedSize);
        break;
#if defined(MARATHON_RECOMP_D3D12)
    case Backend::D3D12:
        g_shaderCache = std::make_unique<uint8_t[]>(g_dxilCacheDecompressedSize);
        ZSTD_decompress(g_shaderCache.get(), g_dxilCacheDecompressedSize, g_compressedDxilCache, g_dxilCacheCompressedSize);
        break;
#elif defined(MARATHON_RECOMP_METAL)
    case Backend::METAL:
        g_shaderCache = std::make_unique<uint8_t[]>(g_airCacheDecompressedSize);
        ZSTD_decompress(g_shaderCache.get(), g_airCacheDecompressedSize, g_compressedAirCache, g_airCacheCompressedSize);
        break;
#endif
    default:
        assert(false);
    }
}

moodycamel::BlockingConcurrentQueue<RenderCommand> g_renderQueue;

static void SetRenderStateUnimplemented(GuestDevice* device, uint32_t value)
{
    LOGF_WARNING("{:x}\n", value);
}

static void SetAlphaTestMode(bool enable)
{
    uint32_t specConstants = 0;
    bool enableAlphaToCoverage = false;

    if (enable)
    {
        enableAlphaToCoverage = Config::TransparencyAntiAliasing && g_renderTarget != nullptr && g_renderTarget->sampleCount != RenderSampleCount::COUNT_1;

        if (enableAlphaToCoverage)
            specConstants = SPEC_CONSTANT_ALPHA_TO_COVERAGE;
        else
            specConstants = SPEC_CONSTANT_ALPHA_TEST;
    }

    specConstants |= (g_pipelineState.specConstants & ~(SPEC_CONSTANT_ALPHA_TEST | SPEC_CONSTANT_ALPHA_TO_COVERAGE));

    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.enableAlphaToCoverage, enableAlphaToCoverage);
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.specConstants, specConstants);
}

static void ProcRenderCommand(const RenderCommands::SetRenderState& args)
{
    uint32_t value = args.value;

    switch (args.type)
    {
    case D3DRS_ZENABLE:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.zEnable, value != 0);
        g_dirtyStates.renderTargetAndDepthStencil |= g_dirtyStates.pipelineState;
        break;
    }
    case D3DRS_ZWRITEENABLE:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.zWriteEnable, value != 0);
        break;
    }
    case D3DRS_STENCILENABLE:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilEnable, value != 0);
        g_dirtyStates.renderTargetAndDepthStencil |= g_dirtyStates.pipelineState;
        break;
    }
    case D3DRS_TWOSIDEDSTENCILMODE:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilTwoSided, value != 0);
        break;
    }
    case D3DRS_ALPHATESTENABLE:
    {
        SetAlphaTestMode(value != 0);
        break;
    }
    case D3DRS_SRCBLEND:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.srcBlend, ConvertBlendMode(value));
        break;
    }
    case D3DRS_DESTBLEND:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.destBlend, ConvertBlendMode(value));
        break;
    }
    case D3DRS_CULLMODE:
    {
        RenderCullMode cullMode;

        switch (value) {
        case D3DCULL_NONE_CCW:
        case D3DCULL_NONE_CW:
            cullMode = RenderCullMode::NONE;
            break;
        case D3DCULL_FRONT_CCW:
        case D3DCULL_FRONT_CW:
            cullMode = RenderCullMode::FRONT;
            break;
        case D3DCULL_BACK_CCW:
        case D3DCULL_BACK_CW:
            cullMode = RenderCullMode::BACK;
            break;
        default:
            assert(false && "Invalid cull mode");
            cullMode = RenderCullMode::NONE;
            break;
        }

        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.cullMode, cullMode);
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.frontFace, value < D3DCULL_NONE_CW ? RenderFrontFace::COUNTER_CLOCKWISE : RenderFrontFace::CLOCKWISE);
        break;
    }
    case D3DRS_ZFUNC:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.zFunc, ConvertCompareFunc(value));
        break;
    }
    case D3DRS_STENCILFUNC:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilFunc, ConvertCompareFunc(value));
        break;
    }
    case D3DRS_STENCILFAIL:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilFail, ConvertStencilOp(value));
        break;
    }
    case D3DRS_STENCILZFAIL:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilZFail, ConvertStencilOp(value));
        break;
    }
    case D3DRS_STENCILPASS:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilPass, ConvertStencilOp(value));
        break;
    }
    case D3DRS_CCW_STENCILFUNC:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilFuncCCW, ConvertCompareFunc(value));
        break;
    }
    case D3DRS_CCW_STENCILFAIL:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilFailCCW, ConvertStencilOp(value));
        break;
    }
    case D3DRS_CCW_STENCILZFAIL:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilZFailCCW, ConvertStencilOp(value));
        break;
    }
    case D3DRS_CCW_STENCILPASS:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilPassCCW, ConvertStencilOp(value));
        break;
    }
    case D3DRS_STENCILREF:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilRef, value);
        break;
    }
    case D3DRS_STENCILMASK:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilMask, value);
        break;
    }
    case D3DRS_STENCILWRITEMASK:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.stencilWriteMask, value);
        break;
    }
    case D3DRS_ALPHAREF:
    {
        SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.alphaThreshold, float(value) / 256.0f);
        break;
    }
    case D3DRS_ALPHABLENDENABLE:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.alphaBlendEnable, value != 0);
        break;
    }
    case D3DRS_BLENDOP:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.blendOp, ConvertBlendOp(value));
        break;
    }
    case D3DRS_SCISSORTESTENABLE:
    {
        // HACK: Ignore scissor test on depth-only draws to allow CSM:3 to properly scale
        if (g_pipelineState.depthStencilFormat == RenderFormat::UNKNOWN || g_pipelineState.renderTargetFormat != RenderFormat::UNKNOWN)
            SetDirtyValue(g_dirtyStates.scissorRect, g_scissorTestEnable, value != 0);
        break;
    }
    case D3DRS_SLOPESCALEDEPTHBIAS:
    {
        if (g_capabilities.dynamicDepthBias)
            SetDirtyValue(g_dirtyStates.depthBias, g_slopeScaledDepthBias, *reinterpret_cast<float*>(&value));
        else 
            SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.slopeScaledDepthBias, *reinterpret_cast<float*>(&value));

        break;
    }
    case D3DRS_DEPTHBIAS:
    {
        if (g_capabilities.dynamicDepthBias)
            SetDirtyValue(g_dirtyStates.depthBias, g_depthBias, int32_t(*reinterpret_cast<float*>(&value) * (1 << 24)));
        else
            SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.depthBias, int32_t(*reinterpret_cast<float*>(&value)* (1 << 24)));

        break;
    }
    case D3DRS_SRCBLENDALPHA:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.srcBlendAlpha, ConvertBlendMode(value));
        break;
    }
    case D3DRS_DESTBLENDALPHA:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.destBlendAlpha, ConvertBlendMode(value));
        break;
    }
    case D3DRS_BLENDOPALPHA:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.blendOpAlpha, ConvertBlendOp(value));
        break;
    }
    case D3DRS_COLORWRITEENABLE:
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.colorWriteEnable, value);
        g_dirtyStates.renderTargetAndDepthStencil |= g_dirtyStates.pipelineState;
        break;
    }
    case D3DRS_CLIPPLANEENABLE:
    {
        // HACK: Only check for clip pane 0
        SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.clipPlaneEnabled, (value & 1) == 1);
    }
    }
}

static std::unique_ptr<RenderShader> g_copyShader;

static std::unique_ptr<RenderShader> g_copyColorShader;
static ankerl::unordered_dense::map<RenderFormat, std::unique_ptr<RenderPipeline>> g_copyColorPipelines;
static std::unique_ptr<RenderPipeline> g_copyDepthPipeline;

static std::unique_ptr<RenderShader> g_resolveMsaaColorShaders[3];
static ankerl::unordered_dense::map<RenderFormat, std::array<std::unique_ptr<RenderPipeline>, 3>> g_resolveMsaaColorPipelines;
static std::unique_ptr<RenderPipeline> g_resolveMsaaDepthPipelines[3];

static std::unique_ptr<RenderShader> g_enhancedBurnoutBlurVSShader;
static std::unique_ptr<RenderShader> g_enhancedBurnoutBlurPSShader;

static std::unique_ptr<RenderShader> g_MoviePSShader;

static std::unique_ptr<GuestShader> g_conditionalSurveyPSShader;

#if defined(MARATHON_RECOMP_D3D12)

#define CREATE_SHADER(NAME) \
    g_device->createShader( \
        (g_backend == Backend::VULKAN) ? g_##NAME##_spirv : g_##NAME##_dxil, \
        (g_backend == Backend::VULKAN) ? sizeof(g_##NAME##_spirv) : sizeof(g_##NAME##_dxil), \
        "shaderMain", \
        (g_backend == Backend::VULKAN) ? RenderShaderFormat::SPIRV : RenderShaderFormat::DXIL)

#elif defined(MARATHON_RECOMP_METAL)

#define CREATE_SHADER(NAME) \
    g_device->createShader( \
        (g_backend == Backend::VULKAN) ? g_##NAME##_spirv : g_##NAME##_air, \
        (g_backend == Backend::VULKAN) ? sizeof(g_##NAME##_spirv) : sizeof(g_##NAME##_air), \
        "shaderMain", \
        (g_backend == Backend::VULKAN) ? RenderShaderFormat::SPIRV : RenderShaderFormat::METAL)

#else

#define CREATE_SHADER(NAME) \
    g_device->createShader(g_##NAME##_spirv, sizeof(g_##NAME##_spirv), "shaderMain", RenderShaderFormat::SPIRV)

#endif

#ifdef _WIN32
static bool DetectWine()
{
    HMODULE dllHandle = GetModuleHandle("ntdll.dll");
    return dllHandle != nullptr && GetProcAddress(dllHandle, "wine_get_version") != nullptr;
}
#endif

static constexpr size_t TEXTURE_DESCRIPTOR_SIZE = 32768;
static constexpr size_t SAMPLER_DESCRIPTOR_SIZE = 1024;

static std::unique_ptr<RenderPipelineLayout> g_imPipelineLayout;
static std::unique_ptr<RenderPipeline> g_imPipeline;
static std::unique_ptr<RenderPipeline> g_imAdditivePipeline;

void Video::ExecuteCopyCommandList(const std::function<void(RenderCommandList*)>& function)
{
    std::lock_guard lock(g_copyMutex);

    g_copyCommandList->begin();
    function(g_copyCommandList.get());
    g_copyCommandList->end();
    g_copyQueue->executeCommandLists(g_copyCommandList.get(), g_copyCommandFence.get());
    g_copyQueue->waitForCommandFence(g_copyCommandFence.get());
}

struct ImGuiPushConstants
{
    ImVec2 boundsMin{};
    ImVec2 boundsMax{};
    ImU32 gradientTopLeft{};
    ImU32 gradientTopRight{};
    ImU32 gradientBottomRight{};
    ImU32 gradientBottomLeft{};
    uint32_t shaderModifier{};
    uint32_t texture2DDescriptorIndex{};
    ImVec2 displaySize{};
    ImVec2 inverseDisplaySize{};
    ImVec2 origin{ 0.0f, 0.0f };
    ImVec2 scale{ 1.0f, 1.0f };
    ImVec2 proceduralOrigin{ 0.0f, 0.0f };
    float outline{};
};

static void CreateImGuiBackend()
{
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    InitImGuiUtils();
    OptionsMenu::Init();
    InstallerWizard::Init();

    ImGui_ImplSDL3_InitForOther(GameWindow::s_pWindow);

    RenderPipelineLayoutBuilder pipelineLayoutBuilder;
    pipelineLayoutBuilder.begin(false, true);

    RenderDescriptorSetBuilder descriptorSetBuilder;
    descriptorSetBuilder.begin();
    descriptorSetBuilder.addTexture(0, TEXTURE_DESCRIPTOR_SIZE);
    descriptorSetBuilder.end(true, TEXTURE_DESCRIPTOR_SIZE);
    pipelineLayoutBuilder.addDescriptorSet(descriptorSetBuilder);

    descriptorSetBuilder.begin();
    descriptorSetBuilder.addSampler(0, SAMPLER_DESCRIPTOR_SIZE);
    descriptorSetBuilder.end(true, SAMPLER_DESCRIPTOR_SIZE);
    pipelineLayoutBuilder.addDescriptorSet(descriptorSetBuilder);

    pipelineLayoutBuilder.addPushConstant(0, 2, sizeof(ImGuiPushConstants), RenderShaderStageFlag::VERTEX | RenderShaderStageFlag::PIXEL);

    pipelineLayoutBuilder.end();
    g_imPipelineLayout = pipelineLayoutBuilder.create(g_device.get());

    auto vertexShader = CREATE_SHADER(imgui_vs);
    auto pixelShader = CREATE_SHADER(imgui_ps);

    RenderInputElement inputElements[3];
    inputElements[0] = RenderInputElement("POSITION", 0, 0, RenderFormat::R32G32_FLOAT, 0, offsetof(ImDrawVert, pos));
    inputElements[1] = RenderInputElement("TEXCOORD", 0, 1, RenderFormat::R32G32_FLOAT, 0, offsetof(ImDrawVert, uv));
    inputElements[2] = RenderInputElement("COLOR", 0, 2, RenderFormat::R8G8B8A8_UNORM, 0, offsetof(ImDrawVert, col));

    RenderInputSlot inputSlot(0, sizeof(ImDrawVert));

    RenderGraphicsPipelineDesc pipelineDesc;
    pipelineDesc.pipelineLayout = g_imPipelineLayout.get();
    pipelineDesc.vertexShader = vertexShader.get();
    pipelineDesc.pixelShader = pixelShader.get();
    pipelineDesc.renderTargetFormat[0] = BACKBUFFER_FORMAT;
    pipelineDesc.renderTargetBlend[0] = RenderBlendDesc::AlphaBlend();
    pipelineDesc.renderTargetCount = 1;
    pipelineDesc.inputElements = inputElements;
    pipelineDesc.inputElementsCount = std::size(inputElements);
    pipelineDesc.inputSlots = &inputSlot;
    pipelineDesc.inputSlotsCount = 1;
    g_imPipeline = g_device->createGraphicsPipeline(pipelineDesc);

    pipelineDesc.renderTargetBlend[0].dstBlend = RenderBlend::ONE;
    g_imAdditivePipeline = g_device->createGraphicsPipeline(pipelineDesc);
}

static void CheckSwapChain()
{
    g_swapChain->setVsyncEnabled(Config::VSync);
    g_swapChainValid &= !g_swapChain->needsResize();

    if (!g_swapChainValid)
    {
        Video::WaitForGPU();
        g_backBuffer->framebuffers.clear();
        PurgeSurfaceVariantFramebuffers();
        g_swapChainValid = g_swapChain->resize();
        Video::s_needsResize = g_swapChainValid;

        while (g_renderSemaphores.size() < g_swapChain->getTextureCount())
            g_renderSemaphores.emplace_back(g_device->createCommandSemaphore());
    }

    if (g_swapChainValid)
    {
        g_swapChainAcquireProfiler.Begin();
        g_swapChainValid = g_swapChain->acquireTexture(g_acquireSemaphores[g_frame].get(), &g_backBufferIndex);
        g_swapChainAcquireProfiler.End();
    }

    if (Video::s_needsResize)
        Video::ComputeViewportDimensions();

    g_backBuffer->width = Video::s_viewportWidth;
    g_backBuffer->height = Video::s_viewportHeight;
}

static void BeginCommandList()
{
    g_renderTarget = g_backBuffer;
    g_depthStencil = nullptr;
    g_framebuffer = nullptr;

    g_pipelineState.renderTargetFormat = BACKBUFFER_FORMAT;
    g_pipelineState.depthStencilFormat = RenderFormat::UNKNOWN;

    if (g_swapChainValid)
    {
        uint32_t width = Video::s_viewportWidth;
        uint32_t height = Video::s_viewportHeight;

        if (g_intermediaryBackBufferTextureWidth != width ||
            g_intermediaryBackBufferTextureHeight != height)
        {
            if (g_intermediaryBackBufferTextureDescriptorIndex == NULL)
                g_intermediaryBackBufferTextureDescriptorIndex = g_textureDescriptorAllocator.allocate();

            Video::WaitForGPU(); // Fine to wait for GPU, this'll only happen during resize.

            g_intermediaryBackBufferTexture = g_device->createTexture(RenderTextureDesc::Texture2D(width, height, 1, BACKBUFFER_FORMAT, RenderTextureFlag::RENDER_TARGET));
            g_textureDescriptorSet->setTexture(g_intermediaryBackBufferTextureDescriptorIndex, g_intermediaryBackBufferTexture.get(), RenderTextureLayout::SHADER_READ);

            g_intermediaryBackBufferTextureWidth = width;
            g_intermediaryBackBufferTextureHeight = height;

            g_backBuffer->framebuffers.clear();
            PurgeSurfaceVariantFramebuffers();

            g_backBuffer->texture = g_intermediaryBackBufferTexture.get();
            g_backBuffer->layout = RenderTextureLayout::UNKNOWN;
            DiscardTexture(g_backBuffer, RenderTextureLayout::COLOR_WRITE);
        }

        g_backBuffer->texture = g_intermediaryBackBufferTexture.get();
    }
    else if (g_intermediaryBackBufferTexture != nullptr)
    {
        g_backBuffer->texture = g_intermediaryBackBufferTexture.get();
    }
    else
    {
        g_backBuffer->texture = g_backBuffer->textureHolder.get();
    }

    g_backBuffer->layout = RenderTextureLayout::UNKNOWN;

    for (size_t i = 0; i < 16; i++)
    {
        g_sharedConstants.texture2DIndices[i] = TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D;
        g_sharedConstants.texture2DArrayIndices[i] = TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D_ARRAY;
        g_sharedConstants.textureCubeIndices[i] = TEXTURE_DESCRIPTOR_NULL_TEXTURE_CUBE;
    }

    memset(g_textures, 0, sizeof(g_textures));

    auto& commandList = g_commandLists[g_frame];

    commandList->begin();
    commandList->resetQueryPool(g_queryPools[g_frame].get(), 0, NUM_QUERIES);
    commandList->writeTimestamp(g_queryPools[g_frame].get(), 0);
    commandList->setGraphicsPipelineLayout(g_pipelineLayout.get());
    commandList->setGraphicsDescriptorSet(g_textureDescriptorSet.get(), 0);
    commandList->setGraphicsDescriptorSet(g_textureDescriptorSet.get(), 1);
    commandList->setGraphicsDescriptorSet(g_textureDescriptorSet.get(), 2);
    commandList->setGraphicsDescriptorSet(g_samplerDescriptorSet.get(), 3);
    commandList->setGraphicsDescriptorSet(g_conditionalSurveyDescriptorSet.get(), 4);
}

template<typename T>
static void ApplyLowEndDefault(ConfigDef<T> &configDef, T newDefault, bool &changed)
{
    if (configDef.IsDefaultValue() && !configDef.IsLoadedFromConfig)
    {
        configDef = newDefault;
        changed = true;
    }
    
    configDef.DefaultValue = newDefault;
}

static void ApplyLowEndDefaults()
{
    bool changed = false;

    ApplyLowEndDefault(Config::AntiAliasing, EAntiAliasing::MSAA2x, changed);
    ApplyLowEndDefault(Config::ShadowResolution, EShadowResolution::x1024, changed);
    ApplyLowEndDefault(Config::ReflectionResolution, EReflectionResolution::Quarter, changed);
    ApplyLowEndDefault(Config::TransparencyAntiAliasing, false, changed);

    if (changed) 
    {
        Config::Save();
    }
}

using RenderInterfaceFunction = std::unique_ptr<RenderInterface>(void);

static bool TryCreateHostDevice(std::vector<RenderInterfaceFunction*>& interfaceFunctions, bool graphicsApiRetry)
{
#ifdef MARATHON_RECOMP_D3D12
    bool allowVulkanRedirection = true;

    if (graphicsApiRetry)
    {
        // If we are attempting to create again after a reboot due to a crash, swap the order.
        g_backend = (g_backend == Backend::VULKAN) ? Backend::D3D12 : Backend::VULKAN;

        // Don't allow redirection to Vulkan if we are retrying after a crash, 
        // so the user can at least boot the game with D3D12 if Vulkan fails to work.
        allowVulkanRedirection = false;
    }
#endif

    for (size_t i = 0; i < interfaceFunctions.size(); i++)
    {
        RenderInterfaceFunction* interfaceFunction = interfaceFunctions[i];

#ifdef MARATHON_RECOMP_D3D12
        // Wrap the device creation in __try/__except to survive from driver crashes.
        __try
#endif
        {
            g_interface = interfaceFunction();
            if (g_interface == nullptr)
            {
                continue;
            }

            g_device = g_interface->createDevice(Config::GraphicsDevice);
            if (g_device != nullptr)
            {
                const RenderDeviceDescription &deviceDescription = g_device->getDescription();
                
#if defined(MARATHON_RECOMP_D3D12)
                if (interfaceFunction == CreateD3D12Interface)
                {
                    if (allowVulkanRedirection)
                    {
                        bool redirectToVulkan = false;

                        // ...
                        // There used to be driver redirections here, but they are all free from Vulkan purgatory for now...
                        // ...

                        if (redirectToVulkan)
                        {
                            g_device.reset();
                            g_interface.reset();

                            // In case Vulkan fails to initialize, we will try D3D12 again afterwards, 
                            // just to get the game to boot. This only really happens in very old Intel GPU drivers.
                            if (g_backend != Backend::VULKAN)
                            {
                                interfaceFunctions.push_back(CreateD3D12Interface);
                                allowVulkanRedirection = false;
                            }

                            continue;
                        }
                    }
                }

                g_backend = (interfaceFunction == CreateVulkanInterface) ? Backend::VULKAN : Backend::D3D12;
#elif defined(MARATHON_RECOMP_METAL)
                g_backend = (interfaceFunction == CreateVulkanInterface) ? Backend::VULKAN : Backend::METAL;
#endif
                // Enable triangle strip workaround if we are on AMD, as there is a bug where
                // restart indices cause triangles to be culled incorrectly. Converting them to degenerate triangles fixes it.
                g_triangleStripWorkaround = (deviceDescription.vendor == RenderDeviceVendor::AMD);

                break;
            }
        }
#ifdef MARATHON_RECOMP_D3D12
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            if (graphicsApiRetry)
            {
                // If we were retrying, and this also failed, then we'll show the user neither of the graphics APIs succeeded.
                return false;
            }
            else
            {
                // If this is the first crash we ran into, reboot and try the other graphics API.
                os::process::StartProcess(os::process::GetExecutablePath(), { "--graphics-api-retry" });
                std::_Exit(0);
            }
        }
#endif
    }

    return true;
}

RenderDevice* Video::GetRenderDevice()
{
    return g_device.get();
}

bool Video::CreateHostDevice(const char* sdlVideoDriver, bool graphicsApiRetry)
{
    for (uint32_t i = 0; i < 16; i++)
        g_inputSlots[i].index = i;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    GameWindow::Init(sdlVideoDriver);

#if defined(MARATHON_RECOMP_D3D12)
    g_backend = (DetectWine() || Config::GraphicsAPI == EGraphicsAPI::Vulkan) ? Backend::VULKAN : Backend::D3D12;
#elif defined(MARATHON_RECOMP_METAL)
    g_backend = Config::GraphicsAPI == EGraphicsAPI::Vulkan ? Backend::VULKAN : Backend::METAL;
#endif

    // Attempt to create the possible backends using a vector of function pointers. Whichever succeeds first will be the chosen API.
    std::vector<RenderInterfaceFunction*> interfaceFunctions;

#ifdef MARATHON_RECOMP_D3D12
    interfaceFunctions.push_back((g_backend == Backend::VULKAN) ? CreateVulkanInterface : CreateD3D12Interface);
    interfaceFunctions.push_back((g_backend == Backend::VULKAN) ? CreateD3D12Interface : CreateVulkanInterface);
#elif defined(MARATHON_RECOMP_METAL)
    interfaceFunctions.push_back((g_backend == Backend::VULKAN) ? CreateVulkanInterface : CreateMetalInterface);
    interfaceFunctions.push_back((g_backend == Backend::VULKAN) ? CreateMetalInterface : CreateVulkanInterface);
#else
    interfaceFunctions.push_back(CreateVulkanInterface);
#endif

    if (!TryCreateHostDevice(interfaceFunctions, graphicsApiRetry))
    {
        return false;
    }

    if (g_device == nullptr)
    {
        return false;
    }

#ifdef MARATHON_RECOMP_D3D12
    if (graphicsApiRetry)
    {
        // If we managed to create a device after retrying it in a reboot, remember the one we picked.
        Config::GraphicsAPI = g_backend == Backend::VULKAN ? EGraphicsAPI::Vulkan : EGraphicsAPI::D3D12;
    }
#endif

    g_capabilities = g_device->getCapabilities();

    LoadEmbeddedResources();

    constexpr uint64_t LowEndMemoryLimit = 2048ULL * 1024ULL * 1024ULL;
    RenderDeviceDescription deviceDescription = g_device->getDescription();
    bool lowEndType = deviceDescription.type != RenderDeviceType::UNKNOWN && deviceDescription.type != RenderDeviceType::DISCRETE;
    bool lowEndMemory = deviceDescription.dedicatedVideoMemory < LowEndMemoryLimit;
    bool lowEndUMA = deviceDescription.type == RenderDeviceType::UNKNOWN && g_capabilities.uma;
    if (lowEndType || lowEndMemory || lowEndUMA)
    {
        // Switch to low end defaults if a non-discrete GPU was detected or a low amount of VRAM was detected.
        // Checking for UMA on D3D12 seems to be a reliable way to detect integrated GPUs.
        ApplyLowEndDefaults();
    }

    const RenderSampleCounts colourSampleCount = g_device->getSampleCountsSupported(RenderFormat::R16G16B16A16_FLOAT);
    const RenderSampleCounts depthSampleCount  = g_device->getSampleCountsSupported(RenderFormat::D32_FLOAT);
    const RenderSampleCounts commonSampleCount = colourSampleCount & depthSampleCount;

    // Disable specific MSAA levels if they are not supported.
    if ((commonSampleCount & RenderSampleCount::COUNT_2) == 0)
        Config::AntiAliasing.InaccessibleValues.emplace(EAntiAliasing::MSAA2x);
    if ((commonSampleCount & RenderSampleCount::COUNT_4) == 0)
        Config::AntiAliasing.InaccessibleValues.emplace(EAntiAliasing::MSAA4x);
    if ((commonSampleCount & RenderSampleCount::COUNT_8) == 0)
        Config::AntiAliasing.InaccessibleValues.emplace(EAntiAliasing::MSAA8x);

    // Set Anti-Aliasing to nearest supported level.
    Config::AntiAliasing.SnapToNearestAccessibleValue(false);

    g_queue = g_device->createCommandQueue(RenderCommandListType::DIRECT);

    for (auto& commandList : g_commandLists)
        commandList = g_queue->createCommandList();

    for (auto& commandFence : g_commandFences)
        commandFence = g_device->createCommandFence();

    for (auto& queryPool : g_queryPools)
        queryPool = g_device->createQueryPool(NUM_QUERIES);

    g_copyQueue = g_device->createCommandQueue(RenderCommandListType::COPY);
    g_copyCommandList = g_copyQueue->createCommandList();
    g_copyCommandFence = g_device->createCommandFence();

    if (g_backend == Backend::D3D12)
    {
        g_discardCommandList = g_queue->createCommandList();
        g_discardCommandFence = g_device->createCommandFence();
    }

    uint32_t bufferCount = 2;

    switch (Config::TripleBuffering)
    {
    case ETripleBuffering::Auto:
        switch (g_backend) {
        case Backend::VULKAN:
            // Defaulting to 3 is fine if presentWait as supported, as the maximum frame latency allowed is only 1.
            bufferCount = g_device->getCapabilities().presentWait ? 3 : 2;
            break;
        case Backend::D3D12:
            // Defaulting to 3 is fine on D3D12 thanks to flip discard model.
            bufferCount = 3;
            break;
        case Backend::METAL:
            bufferCount = 2;
            break;
        }

        break;
    case ETripleBuffering::On:
        bufferCount = 3;
        break;
    case ETripleBuffering::Off:
        bufferCount = 2;
        break;
    }

    RenderSwapChainDesc swapChainDesc;
    swapChainDesc.renderWindow = GameWindow::s_renderWindow;
    swapChainDesc.textureCount = bufferCount;
    swapChainDesc.format = BACKBUFFER_FORMAT;
    swapChainDesc.maxFrameLatency = Config::MaxFrameLatency;
    swapChainDesc.enablePresentWait = g_capabilities.presentWait;

    g_swapChain = g_queue->createSwapChain(swapChainDesc);
    g_swapChain->setVsyncEnabled(Config::VSync);
    g_swapChainValid = !g_swapChain->needsResize();

    for (auto& acquireSemaphore : g_acquireSemaphores)
        acquireSemaphore = g_device->createCommandSemaphore();

    g_renderSemaphores.resize(g_swapChain->getTextureCount());
    
    for (auto& renderSemaphore : g_renderSemaphores)
        renderSemaphore = g_device->createCommandSemaphore();

    RenderPipelineLayoutBuilder pipelineLayoutBuilder;
    pipelineLayoutBuilder.begin(false, true);
    
    RenderDescriptorSetBuilder descriptorSetBuilder;
    descriptorSetBuilder.begin();
    descriptorSetBuilder.addTexture(0, TEXTURE_DESCRIPTOR_SIZE);
    descriptorSetBuilder.end(true, TEXTURE_DESCRIPTOR_SIZE);
    
    g_textureDescriptorSet = descriptorSetBuilder.create(g_device.get());
    
    for (size_t i = 0; i < TEXTURE_DESCRIPTOR_NULL_COUNT; i++)
    {
        auto& texture = g_blankTextures[i];
        auto& textureView = g_blankTextureViews[i];

        RenderTextureDesc desc;
        desc.width = 1;
        desc.height = 1;
        desc.depth = 1;
        desc.mipLevels = 1;
        desc.format = RenderFormat::R8_UNORM;

        RenderTextureViewDesc viewDesc;
        viewDesc.format = desc.format;
        viewDesc.componentMapping = RenderComponentMapping(RenderSwizzle::ZERO, RenderSwizzle::ZERO, RenderSwizzle::ZERO, RenderSwizzle::ZERO);
        viewDesc.mipLevels = 1;

        switch (i)
        {
        case TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D:
            desc.dimension = RenderTextureDimension::TEXTURE_2D;
            desc.arraySize = 1;
            viewDesc.dimension = RenderTextureViewDimension::TEXTURE_2D;
            break;

        case TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D_ARRAY:
            desc.dimension = RenderTextureDimension::TEXTURE_2D;
            desc.arraySize = 1;
            viewDesc.dimension = RenderTextureViewDimension::TEXTURE_2D;
            break;

        case TEXTURE_DESCRIPTOR_NULL_TEXTURE_CUBE:
            desc.dimension = RenderTextureDimension::TEXTURE_2D;
            desc.arraySize = 6;
            desc.flags = RenderTextureFlag::CUBE;
            viewDesc.dimension = RenderTextureViewDimension::TEXTURE_CUBE;
            break;

        default:
            assert(false && "Unknown null descriptor dimension");
            break;
        }

        texture = g_device->createTexture(desc);
        textureView = texture->createTextureView(viewDesc);

        g_textureDescriptorSet->setTexture(i, texture.get(), RenderTextureLayout::SHADER_READ, textureView.get());
    }

    pipelineLayoutBuilder.addDescriptorSet(descriptorSetBuilder);
    pipelineLayoutBuilder.addDescriptorSet(descriptorSetBuilder);
    pipelineLayoutBuilder.addDescriptorSet(descriptorSetBuilder);
    
    descriptorSetBuilder.begin();
    descriptorSetBuilder.addSampler(0, SAMPLER_DESCRIPTOR_SIZE);
    descriptorSetBuilder.end(true, SAMPLER_DESCRIPTOR_SIZE);
    
    g_samplerDescriptorSet = descriptorSetBuilder.create(g_device.get());
    auto& [descriptorIndex, sampler] = g_samplerStates[XXH3_64bits(&g_samplerDescs[0], sizeof(RenderSamplerDesc))];
    descriptorIndex = 1;
    sampler = g_device->createSampler(g_samplerDescs[0]);
    g_samplerDescriptorSet->setSampler(0, sampler.get());

    pipelineLayoutBuilder.addDescriptorSet(descriptorSetBuilder);

    RenderBufferDesc conditionalSurveyBufferDesc;
    conditionalSurveyBufferDesc.size = CONDITIONAL_SURVEY_MAX * sizeof(uint32_t);
    conditionalSurveyBufferDesc.heapType = RenderHeapType::DEFAULT;
    conditionalSurveyBufferDesc.flags = RenderBufferFlag::STORAGE | RenderBufferFlag::UNORDERED_ACCESS;
    g_conditionalSurveyBuffer = g_device->createBuffer(conditionalSurveyBufferDesc);

    RenderDescriptorSetBuilder conditionalSurveyDescriptorSetBuilder;
    conditionalSurveyDescriptorSetBuilder.begin();
    conditionalSurveyDescriptorSetBuilder.addReadWriteStructuredBuffer(0);
    conditionalSurveyDescriptorSetBuilder.end();
    g_conditionalSurveyDescriptorSet = conditionalSurveyDescriptorSetBuilder.create(g_device.get());

    RenderBufferStructuredView conditionalSurveyStructuredView(sizeof(uint32_t));
    g_conditionalSurveyDescriptorSet->setBuffer(0, g_conditionalSurveyBuffer.get(), 0, &conditionalSurveyStructuredView);

    pipelineLayoutBuilder.addDescriptorSet(conditionalSurveyDescriptorSetBuilder);

    if (g_backend != Backend::D3D12)
    {
        pipelineLayoutBuilder.addPushConstant(0, 4, 24, RenderShaderStageFlag::VERTEX | RenderShaderStageFlag::PIXEL);
    }
    else
    {
        pipelineLayoutBuilder.addRootDescriptor(0, 4, RenderRootDescriptorType::CONSTANT_BUFFER);
        pipelineLayoutBuilder.addRootDescriptor(1, 4, RenderRootDescriptorType::CONSTANT_BUFFER);
        pipelineLayoutBuilder.addRootDescriptor(2, 4, RenderRootDescriptorType::CONSTANT_BUFFER);
        pipelineLayoutBuilder.addPushConstant(3, 4, 4, RenderShaderStageFlag::PIXEL); // For copy/resolve shaders.
    }
    pipelineLayoutBuilder.end();
    
    g_pipelineLayout = pipelineLayoutBuilder.create(g_device.get());

    g_copyShader = CREATE_SHADER(copy_vs);
    g_copyColorShader = CREATE_SHADER(copy_color_ps);
    auto copyDepthShader = CREATE_SHADER(copy_depth_ps);

    RenderGraphicsPipelineDesc desc;
    desc.pipelineLayout = g_pipelineLayout.get();
    desc.vertexShader = g_copyShader.get();
    desc.pixelShader = copyDepthShader.get();
    desc.depthFunction = RenderComparisonFunction::ALWAYS;
    desc.depthEnabled = true;
    desc.depthWriteEnabled = true;
    desc.depthTargetFormat = RenderFormat::D32_FLOAT_S8_UINT;
    g_copyDepthPipeline = g_device->createGraphicsPipeline(desc);

    g_resolveMsaaColorShaders[0] = CREATE_SHADER(resolve_msaa_color_2x);
    g_resolveMsaaColorShaders[1] = CREATE_SHADER(resolve_msaa_color_4x);
    g_resolveMsaaColorShaders[2] = CREATE_SHADER(resolve_msaa_color_8x);

    for (size_t i = 0; i < std::size(g_resolveMsaaDepthPipelines); i++)
    {
        std::unique_ptr<RenderShader> pixelShader;
        switch (i)
        {
        case 0:
            pixelShader = CREATE_SHADER(resolve_msaa_depth_2x);
            break;
        case 1:
            pixelShader = CREATE_SHADER(resolve_msaa_depth_4x);
            break;
        case 2:
            pixelShader = CREATE_SHADER(resolve_msaa_depth_8x);
            break;
        }

        desc = {};
        desc.pipelineLayout = g_pipelineLayout.get();
        desc.vertexShader = g_copyShader.get();
        desc.pixelShader = pixelShader.get();
        desc.depthFunction = RenderComparisonFunction::ALWAYS;
        desc.depthEnabled = true;
        desc.depthWriteEnabled = true;
        desc.depthTargetFormat = RenderFormat::D32_FLOAT_S8_UINT;
        g_resolveMsaaDepthPipelines[i] = g_device->createGraphicsPipeline(desc);
    }

    g_enhancedBurnoutBlurVSShader = CREATE_SHADER(enhanced_burnout_blur_vs);
    g_enhancedBurnoutBlurPSShader = CREATE_SHADER(enhanced_burnout_blur_ps);
    g_MoviePSShader = CREATE_SHADER(movie_ps);

    g_conditionalSurveyPSShader = std::make_unique<GuestShader>(ResourceType::PixelShader);
    g_conditionalSurveyPSShader->shader = CREATE_SHADER(conditional_survey_ps);

    CreateImGuiBackend();

    auto gammaCorrectionShader = CREATE_SHADER(gamma_correction_ps);

    desc = {};
    desc.pipelineLayout = g_pipelineLayout.get();
    desc.vertexShader = g_copyShader.get();
    desc.pixelShader = gammaCorrectionShader.get();
    desc.renderTargetFormat[0] = BACKBUFFER_FORMAT;
    desc.renderTargetBlend[0] = RenderBlendDesc::Copy();
    desc.renderTargetCount = 1;
    g_gammaCorrectionPipeline = g_device->createGraphicsPipeline(desc);

    // NOTE: We initially allocate this on host memory to make the installer work, even if the 4 GB memory allocation fails.
    g_backBufferHolder = std::make_unique<GuestSurface>(ResourceType::RenderTarget);

    g_backBuffer = g_backBufferHolder.get();
    g_backBuffer->width = 1280;
    g_backBuffer->height = 720;
    g_backBuffer->format = BACKBUFFER_FORMAT;
    g_backBuffer->textureHolder = g_device->createTexture(RenderTextureDesc::Texture2D(1, 1, 1, BACKBUFFER_FORMAT, RenderTextureFlag::RENDER_TARGET));

    Video::ComputeViewportDimensions();
    CheckSwapChain();
    BeginCommandList();

    RenderTextureBarrier blankTextureBarriers[TEXTURE_DESCRIPTOR_NULL_COUNT];
    for (size_t i = 0; i < TEXTURE_DESCRIPTOR_NULL_COUNT; i++)
        blankTextureBarriers[i] = RenderTextureBarrier(g_blankTextures[i].get(), RenderTextureLayout::SHADER_READ);

    g_commandLists[g_frame]->barriers(RenderBarrierStage::NONE, blankTextureBarriers, std::size(blankTextureBarriers));

    return true;
}

static uint32_t g_waitForGPUCount = 0;

void Video::WaitForGPU()
{
    g_waitForGPUCount++;

    // Wait for all queued frames to finish.
    for (size_t i = 0; i < NUM_FRAMES; i++)
    {
        if (g_commandListStates[i])
        {
            g_queue->waitForCommandFence(g_commandFences[i].get());
            g_commandListStates[i] = false;
        }
    }

    // Execute an empty command list and wait for it to end to guarantee that any remaining presentation has finished.
    g_commandLists[0]->begin();
    g_commandLists[0]->end();
    g_queue->executeCommandLists(g_commandLists[0].get(), g_commandFences[0].get());
    g_queue->waitForCommandFence(g_commandFences[0].get());
}

static const GuestRenderState g_implementedRenderStates[] =
{
    D3DRS_ZENABLE,
    D3DRS_ZWRITEENABLE,
    D3DRS_ALPHATESTENABLE,
    D3DRS_SRCBLEND,
    D3DRS_DESTBLEND,
    D3DRS_CULLMODE,
    D3DRS_ZFUNC,
    D3DRS_ALPHAREF,
    D3DRS_ALPHABLENDENABLE,
    D3DRS_BLENDOP,
    D3DRS_SCISSORTESTENABLE,
    D3DRS_SLOPESCALEDEPTHBIAS,
    D3DRS_DEPTHBIAS,
    D3DRS_SRCBLENDALPHA,
    D3DRS_DESTBLENDALPHA,
    D3DRS_BLENDOPALPHA,
    D3DRS_COLORWRITEENABLE,
    D3DRS_STENCILENABLE,
    D3DRS_TWOSIDEDSTENCILMODE,
    D3DRS_STENCILFAIL,
    D3DRS_STENCILZFAIL,
    D3DRS_STENCILPASS,
    D3DRS_STENCILFUNC,
    D3DRS_STENCILREF,
    D3DRS_STENCILMASK,
    D3DRS_STENCILWRITEMASK,
    D3DRS_CCW_STENCILFAIL,
    D3DRS_CCW_STENCILZFAIL,
    D3DRS_CCW_STENCILPASS,
    D3DRS_CCW_STENCILFUNC,
    D3DRS_CLIPPLANEENABLE
};

static uint32_t getSetAddress(uint32_t base, int index) {
    uint32_t entryOffset = index * 0xC;
    uint32_t entryAddress = base + entryOffset;
    uint32_t setAddress = entryAddress + sizeof(uint32_t);
    return setAddress;
}

static uint32_t CreateDevice(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, be<uint32_t>* a6)
{
    g_xdbfTextureCache = std::unordered_map<uint16_t, GuestTexture*>();

    for (auto &achievement : g_xdbfWrapper.GetAchievements(XDBF_LANGUAGE_ENGLISH))
    {
        if (!achievement.pImageBuffer || !achievement.ImageBufferSize)
            continue;

        g_xdbfTextureCache[achievement.ID] =
            LoadTexture((uint8_t *)achievement.pImageBuffer, achievement.ImageBufferSize).release();
    }

    // Move backbuffer to guest memory.
    assert(!g_memory.IsInMemoryRange(g_backBuffer) && g_backBufferHolder != nullptr);
    g_backBuffer = g_userHeap.AllocPhysical<GuestSurface>(std::move(*g_backBufferHolder));

    // Check for stale reference. BeginCommandList() gets called before CreateDevice() which is where the assignment happens.
    if (g_renderTarget == g_backBufferHolder.get()) g_renderTarget = g_backBuffer;
    if (g_depthStencil == g_backBufferHolder.get()) g_depthStencil = g_backBuffer;

    // Free the host backbuffer.
    g_backBufferHolder = nullptr;

    auto device = g_userHeap.AllocPhysical<GuestDevice>();
    memset(device, 0, sizeof(*device));

    // Append render state functions to the end of guest function table.
    uint32_t functionOffsetUnimplemented = PPC_CODE_BASE + PPC_CODE_SIZE;
    g_memory.InsertFunction(functionOffsetUnimplemented, HostToGuestFunction<SetRenderStateUnimplemented>);
    
    uint32_t functionOffset = 0x82B79868;
    for (size_t i = 0; i < std::size(device->setRenderStateFunctions); i++) {
        device->setRenderStateFunctions[i] = functionOffsetUnimplemented;
    }

    // Point implemented states at the game's own setters, which are hooked by address.
    for (auto state : g_implementedRenderStates)
    {
        auto funcOffset = getSetAddress(functionOffset, state / 4);
        uint32_t addr = __builtin_bswap32(*(uint32_t*)g_memory.Translate(funcOffset));
        device->setRenderStateFunctions[state / 4] = addr;
    }

    for (size_t i = 0; i < std::size(device->setSamplerStateFunctions); i++)
        device->setSamplerStateFunctions[i] = *static_cast<uint32_t*>(g_memory.Translate(0x82B79CFC + i * 0xC));

    device->viewport.width = 1280.0f;
    device->viewport.height = 720.0f;
    device->viewport.maxZ = 1.0f;

    *a6 = g_memory.MapVirtual(device);

    return 0;
}

static void ProcRenderCommand(const RenderCommands::DestructResource& args)
{
    g_tempResources[g_renderFrame].push_back(args.resource);
}

static void ProcRenderCommand(const RenderCommands::UnlockTextureRect& args)
{
    AddBarrier(args.texture, RenderTextureLayout::COPY_DEST);
    FlushBarriers();

    uint32_t pitch = ComputeTexturePitch(args.texture);
    uint32_t slicePitch = pitch * args.texture->height;

    auto allocation = g_uploadAllocators[g_frame].allocate(slicePitch, PLACEMENT_ALIGNMENT);
    memcpy(allocation.memory, args.texture->mappedMemory, slicePitch);

    g_commandLists[g_frame]->copyTextureRegion(
        RenderTextureCopyLocation::Subresource(args.texture->texture, 0),
        RenderTextureCopyLocation::PlacedFootprint(allocation.buffer, args.texture->format, args.texture->width, args.texture->height, 1, pitch / RenderFormatSize(args.texture->format), allocation.offset));
}

static const char *DeviceTypeName(RenderDeviceType type)
{
    switch (type) 
    {
    case RenderDeviceType::INTEGRATED:
        return "Integrated";
    case RenderDeviceType::DISCRETE:
        return "Discrete";
    case RenderDeviceType::VIRTUAL:
        return "Virtual";
    case RenderDeviceType::CPU:
        return "CPU";
    default:
        return "Unknown";
    }
}

static void DrawProfiler()
{
    const auto toggleProfiler = SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_F1] != 0;

    if (!g_profilerWasToggled && toggleProfiler)
    {
        g_profilerVisible = !g_profilerVisible;

        GameWindow::SetFullscreenCursorVisibility(App::s_isInit ? g_profilerVisible : true);
    }

    g_profilerWasToggled = toggleProfiler;

    if (!g_profilerVisible)
        return;

    const auto pFont = g_pFntRodin;
    const auto defaultFontScale = pFont->Scale;
    pFont->Scale = ImGui::GetDefaultFont()->FontSize / pFont->FontSize;
    ImGui::PushFont(pFont);

#define IMGUI_GENERIC_ROW(name, value, ...) \
    do                                      \
    {                                       \
        ImGui::TableNextColumn();           \
        ImGui::Text(name);                  \
        ImGui::TableNextColumn();           \
        ImGui::Text(value, __VA_ARGS__);    \
    }                                       \
    while (false);

    if (ImGui::Begin("Profiler", &g_profilerVisible))
    {
        g_applicationValues[g_profilerValueIndex] = App::s_deltaTime * 1000.0;

        const auto applicationAvg = std::accumulate(g_applicationValues, g_applicationValues + PROFILER_VALUE_COUNT, 0.0) / PROFILER_VALUE_COUNT;
        const auto gpuFrameAvg = g_gpuFrameProfiler.UpdateAndReturnAverage();
        const auto presentAvg = g_presentProfiler.UpdateAndReturnAverage();
        const auto frameFenceAvg = g_frameFenceProfiler.UpdateAndReturnAverage();
        const auto presentWaitAvg = g_presentWaitProfiler.UpdateAndReturnAverage();
        const auto swapChainAcquireAvg = g_swapChainAcquireProfiler.UpdateAndReturnAverage();

        if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImPlot::BeginPlot("Frame Time"))
            {
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 20.0);
                ImPlot::SetupAxis(ImAxis_Y1, "ms", ImPlotAxisFlags_None);
                ImPlot::PlotLine<double>("Application", g_applicationValues, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
                ImPlot::PlotLine<double>("GPU Frame", g_gpuFrameProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
                ImPlot::PlotLine<double>("Present", g_presentProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
                ImPlot::PlotLine<double>("Present Wait", g_presentWaitProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
                ImPlot::PlotLine<double>("Frame Fence", g_frameFenceProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
                ImPlot::PlotLine<double>("Swap Chain Acquire", g_swapChainAcquireProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
                ImPlot::EndPlot();
            }

            g_profilerValueIndex = (g_profilerValueIndex + 1) % PROFILER_VALUE_COUNT;

            if (ImGui::BeginTable("Performance", 5))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Current Time");
                ImGui::TableSetupColumn("Average Time");
                ImGui::TableSetupColumn("Current FPS");
                ImGui::TableSetupColumn("Average FPS");
                ImGui::TableHeadersRow();

                auto drawPerfRow = [](const char* name, double ms, double msAvg, bool showFPS = false, double fps = 0, double fpsAvg = 0)
                {
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", name);
                    ImGui::TableNextColumn();
                    ImGui::Text("%g ms", ms);
                    ImGui::TableNextColumn();
                    ImGui::Text("%g ms", msAvg);
                    ImGui::TableNextColumn();

                    if (showFPS)
                        ImGui::Text("%g FPS", fps);

                    ImGui::TableNextColumn();

                    if (showFPS)
                        ImGui::Text("%g FPS", fpsAvg);
                };

                // -------- Name ---------------- Current Time --------------------------- Average Time -------- Current FPS ----------------------------- Average FPS ---------- //
                drawPerfRow("Application",        App::s_deltaTime * 1000.0,               applicationAvg, true, 1.0 / App::s_deltaTime,                   1000.0 / applicationAvg);
                drawPerfRow("GPU Frame",          g_gpuFrameProfiler.value.load(),         gpuFrameAvg,    true, 1000.0 / g_gpuFrameProfiler.value.load(), 1000.0 / gpuFrameAvg   );
                drawPerfRow("Present",            g_presentProfiler.value.load(),          presentAvg,     true, 1000.0 / g_presentProfiler.value.load(),  1000.0 / presentAvg    );
                drawPerfRow("Present Wait",       g_presentWaitProfiler.value.load(),      presentWaitAvg                                                                         );
                drawPerfRow("Frame Fence",        g_frameFenceProfiler.value.load(),       frameFenceAvg                                                                          );
                drawPerfRow("Swap Chain Acquire", g_swapChainAcquireProfiler.value.load(), swapChainAcquireAvg                                                                    );

                ImGui::EndTable();
            }

            ImGui::Separator();

            ImGui::Checkbox("Show FPS", &Config::ShowFPS.Value);
        }

        if (g_userHeap.heap && g_userHeap.physicalHeap)
        {
            if (ImGui::CollapsingHeader("Memory", ImGuiTreeNodeFlags_DefaultOpen))
            {
                std::lock_guard heapLock(g_userHeap.mutex);
                O1HeapDiagnostics heapDiagnostics = o1heapGetDiagnostics(g_userHeap.heap);

                std::lock_guard physicalHeapLock(g_userHeap.physicalMutex);
                O1HeapDiagnostics physicalHeapDiagnostics = o1heapGetDiagnostics(g_userHeap.physicalHeap);

                if (ImGui::BeginTable("Memory", 2))
                {
                    IMGUI_GENERIC_ROW("Heap Allocated", "%.2f MiB", double(heapDiagnostics.allocated / (1024.0 * 1024.0)));
                    IMGUI_GENERIC_ROW("Physical Heap Allocated", "%.2f MiB", double(physicalHeapDiagnostics.allocated / (1024.0 * 1024.0)));

                    ImGui::EndTable();
                }
            }
        }

        if (ImGui::CollapsingHeader("GPU", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const char* backend{};

            switch (g_backend)
            {
                case Backend::VULKAN:
                    backend = "Vulkan";
                    break;

                case Backend::D3D12:
                    backend = "D3D12";
                    break;

                case Backend::METAL:
                    backend = "Metal";
                    break;

                default:
                    backend = "Unknown";
                    break;
            }

            if (ImGui::BeginTable("GPU", 2))
            {
                IMGUI_GENERIC_ROW("API", "%s", backend);

                if (const auto pSDLVideoDriver = SDL_GetCurrentVideoDriver())
                    IMGUI_GENERIC_ROW("SDL Video Driver", "%s", pSDLVideoDriver);

                IMGUI_GENERIC_ROW("Device", "%s", g_device->getDescription().name.c_str());
                IMGUI_GENERIC_ROW("Device Type", "%s", DeviceTypeName(g_device->getDescription().type));
                IMGUI_GENERIC_ROW("VRAM", "%.2f MiB", double(g_device->getDescription().dedicatedVideoMemory) / (1024.0 * 1024.0));
                IMGUI_GENERIC_ROW("GPU Waits", "%u", g_waitForGPUCount);
                IMGUI_GENERIC_ROW("Buffer Uploads", "%u", g_bufferUploadCount.load());

                IMGUI_GENERIC_ROW("Resolution", "%ux%u (%ux%u)",
                    Video::s_viewportWidth, Video::s_viewportHeight,
                    uint32_t(round(Video::s_viewportWidth * Config::ResolutionScale)),
                    uint32_t(round(Video::s_viewportHeight * Config::ResolutionScale)));

                ImGui::EndTable();
            }

            ImGui::Separator();

#ifdef ASYNC_PSO_DEBUG
            if (ImGui::TreeNode("Async PSO"))
            {
                ImGui::Indent();

                if (ImGui::BeginTable("Async PSO", 2))
                {
                    IMGUI_GENERIC_ROW("Pipelines created in render thread", "%u", g_pipelinesCreatedInRenderThread.load());
                    IMGUI_GENERIC_ROW("Pipelines created asynchronously", "%u", g_pipelinesCreatedAsynchronously.load());
                    IMGUI_GENERIC_ROW("Pipelines dropped", "%u", g_pipelinesDropped.load());
                    IMGUI_GENERIC_ROW("Pipelines currently compiling", "%u", g_pipelinesCurrentlyCompiling.load());
                    IMGUI_GENERIC_ROW("Compiling pipeline tasks", "%u", g_compilingPipelineTaskCount.load());
                    IMGUI_GENERIC_ROW("Pending pipeline tasks", "%u", g_pendingPipelineTaskCount.load());

                    ImGui::EndTable();
                }

                ImGui::Separator();
                
                if (ImGui::TreeNode("Pipeline States"))
                {
                    ImGui::Indent();

                    std::lock_guard lock(g_debugMutex);
                    ImGui::TextUnformatted(g_pipelineDebugText.c_str());

                    ImGui::Unindent();
                    ImGui::TreePop();
                }

                ImGui::Unindent();
                ImGui::TreePop();
            }
#endif

            if (ImGui::TreeNode("Devices"))
            {
                ImGui::Indent();

                if (ImGui::BeginTable("Devices", 2))
                {
                    auto deviceIndex = 0;

                    for (const auto& deviceName : g_interface->getDeviceNames())
                    {
                        ImGui::TableNextColumn();
                        ImGui::Text("Device #%d", deviceIndex++);
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", deviceName.c_str());
                        ImGui::SameLine();
                    }

                    ImGui::EndTable();
                }

                ImGui::Unindent();
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Features"))
            {
                ImGui::Indent();

                if (ImGui::BeginTable("Features", 2))
                {
                    IMGUI_GENERIC_ROW("Dynamic Depth Bias", "%s", g_capabilities.dynamicDepthBias ? "Supported" : "Unsupported");
                    IMGUI_GENERIC_ROW("GPU Upload Heap", "%s", g_capabilities.gpuUploadHeap ? "Supported" : "Unsupported");
                    IMGUI_GENERIC_ROW("Hardware Resolve Modes", "%s", g_capabilities.resolveModes ? "Supported" : "Unsupported");
                    IMGUI_GENERIC_ROW("Present Wait", "%s", g_capabilities.presentWait ? "Supported" : "Unsupported");
                    IMGUI_GENERIC_ROW("Triangle Fan", "%s", g_capabilities.triangleFan ? "Supported" : "Unsupported");
                    IMGUI_GENERIC_ROW("Triangle Strip Workaround", "%s", g_triangleStripWorkaround ? "Enabled" : "Disabled");
                    IMGUI_GENERIC_ROW("UMA", "%s", g_capabilities.uma ? "Supported" : "Unsupported");

                    ImGui::EndTable();
                }

                ImGui::Unindent();
                ImGui::TreePop();
            }
        }
    }

#undef IMGUI_GENERIC_ROW

    ImGui::End();
    ImGui::PopFont();

    pFont->Scale = defaultFontScale;
}

static void DrawFPS()
{
    if (!Config::ShowFPS)
        return;

    double time = ImGui::GetTime();
    static double updateTime = time;
    static double fps = 0;
    static double totalDeltaTime = 0.0;
    static uint32_t totalDeltaCount = 0;

    totalDeltaTime += g_presentProfiler.value.load();
    totalDeltaCount++;

    if (time - updateTime >= 1.0f)
    {
        fps = 1000.0 / std::max(totalDeltaTime / double(totalDeltaCount), 1.0);
        updateTime = time;
        totalDeltaTime = 0.0;
        totalDeltaCount = 0;
    }

    auto drawList = ImGui::GetBackgroundDrawList();

    auto fmt = fmt::format("FPS: {:.2f}", fps);
    auto fontSize = Scale(10);
    auto textSize = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0, fmt.c_str());

    ImVec2 min = { Scale(40), Scale(30) };
    ImVec2 max = { min.x + std::max(Scale(75), textSize.x + Scale(10)), min.y + Scale(15) };
    ImVec2 textPos = { min.x + Scale(2), CENTRE_TEXT_VERT(min, max, textSize) + Scale(0.2f) };

    drawList->AddRectFilled(min, max, IM_COL32(0, 0, 0, 200));
    drawList->AddText(g_pFntRodin, fontSize, textPos, IM_COL32_WHITE, fmt.c_str());
}

static std::vector<std::function<void()>> g_imguiDrawQueue{};

void Video::EnqueueImGuiDraw(const std::function<void()>& in_rEvent)
{
    g_imguiDrawQueue.push_back(in_rEvent);
}

static void DrawImGui()
{
    if (!ImFontAtlasSnapshot::IsInitialised)
        return;

    ImGui_ImplSDL3_NewFrame();

    auto& io = ImGui::GetIO();
    io.DisplaySize = { float(Video::s_viewportWidth), float(Video::s_viewportHeight) };

    // ImGui doesn't know that we center the screen for specific aspect ratio
    // settings, which causes mouse events to not work correctly. To fix this, 
    // we can adjust the mouse events before ImGui processes them.
    uint32_t width = g_swapChain->getWidth();
    uint32_t height = g_swapChain->getHeight();
    float mousePosScaleX = float(width) / float(GameWindow::s_width);
    float mousePosScaleY = float(height) / float(GameWindow::s_height);
    float mousePosOffsetX = (width - Video::s_viewportWidth) / 2.0f;
    float mousePosOffsetY = (height - Video::s_viewportHeight) / 2.0f;
    for (int i = 0; i < io.Ctx->InputEventsQueue.Size; i++)
    {
        auto& e = io.Ctx->InputEventsQueue[i];
        if (e.Type == ImGuiInputEventType_MousePos)
        {
            if (e.MousePos.PosX != -FLT_MAX)
            {
                e.MousePos.PosX *= mousePosScaleX;
                e.MousePos.PosX -= mousePosOffsetX;
            }

            if (e.MousePos.PosY != -FLT_MAX)
            {
                e.MousePos.PosY *= mousePosScaleY;
                e.MousePos.PosY -= mousePosOffsetY;
            }
        }
    }

    ImGui::NewFrame();

    ResetImGuiCallbacks();

    UpdateImGuiUtils();
    AchievementMenu::Draw();
    OptionsMenu::Draw();
    InstallerWizard::Draw();
    ButtonWindow::Draw();
    MessageWindow::Draw();
    AchievementOverlay::Draw();
    Fader::Draw();
    BlackBar::Draw();

    for (const auto& rEvent : g_imguiDrawQueue)
        rEvent();

    g_imguiDrawQueue.clear();

    assert(ImGui::GetBackgroundDrawList()->_ClipRectStack.Size == 1 && "Some clip rects were not removed from the stack!");

    DrawFPS();
    DrawProfiler();
    ImGui::Render();

    auto drawData = ImGui::GetDrawData();
    if (drawData->CmdListsCount != 0)
        g_renderQueue.enqueue(RenderCommands::DrawImGui{});
}

static void SetFramebuffer(GuestSurface *renderTarget, GuestSurface *depthStencil, bool settingForClear);

static void ProcRenderCommand(const RenderCommands::DrawImGui& args)
{
    // Make sure the backbuffer is the current target.
    AddBarrier(g_backBuffer, RenderTextureLayout::COLOR_WRITE);
    FlushBarriers();
    SetFramebuffer(g_backBuffer, nullptr, false);

    auto& commandList = g_commandLists[g_frame];
    auto pipeline = g_imPipeline.get();

    commandList->setGraphicsPipelineLayout(g_imPipelineLayout.get());
    commandList->setPipeline(pipeline);
    commandList->setGraphicsDescriptorSet(g_textureDescriptorSet.get(), 0);
    commandList->setGraphicsDescriptorSet(g_samplerDescriptorSet.get(), 1);

    auto& drawData = *ImGui::GetDrawData();
    commandList->setViewports(RenderViewport(drawData.DisplayPos.x, drawData.DisplayPos.y, drawData.DisplaySize.x, drawData.DisplaySize.y));

    ImGuiPushConstants pushConstants{};
    pushConstants.displaySize = drawData.DisplaySize;
    pushConstants.inverseDisplaySize = { 1.0f / drawData.DisplaySize.x, 1.0f / drawData.DisplaySize.y };
    commandList->setGraphicsPushConstants(0, &pushConstants);

    size_t pushConstantRangeMin = ~0;
    size_t pushConstantRangeMax = 0;

    auto setPushConstants = [&](void* destination, const void* source, size_t size)
        {
            bool dirty = memcmp(destination, source, size) != 0;

            memcpy(destination, source, size);

            if (dirty)
            {
                size_t offset = reinterpret_cast<size_t>(destination) - reinterpret_cast<size_t>(&pushConstants);
                pushConstantRangeMin = std::min(pushConstantRangeMin, offset);
                pushConstantRangeMax = std::max(pushConstantRangeMax, offset + size);
            }
        };

    ImRect clipRect{};

    for (int i = 0; i < drawData.CmdListsCount; i++)
    {
        auto& drawList = drawData.CmdLists[i];

        auto vertexBufferAllocation = g_uploadAllocators[g_frame].allocate<false>(drawList->VtxBuffer.Data, drawList->VtxBuffer.Size * sizeof(ImDrawVert), alignof(ImDrawVert));
        auto indexBufferAllocation = g_uploadAllocators[g_frame].allocate<false>(drawList->IdxBuffer.Data, drawList->IdxBuffer.Size * sizeof(uint16_t), alignof(uint16_t));

        const RenderVertexBufferView vertexBufferView(vertexBufferAllocation.buffer->at(vertexBufferAllocation.offset), drawList->VtxBuffer.Size * sizeof(ImDrawVert));
        const RenderInputSlot inputSlot(0, sizeof(ImDrawVert));
        commandList->setVertexBuffers(0, &vertexBufferView, 1, &inputSlot);

        const RenderIndexBufferView indexBufferView(indexBufferAllocation.buffer->at(indexBufferAllocation.offset), drawList->IdxBuffer.Size * sizeof(uint16_t), RenderFormat::R16_UINT);
        commandList->setIndexBuffer(&indexBufferView);

        for (int j = 0; j < drawList->CmdBuffer.Size; j++)
        {
            auto& drawCmd = drawList->CmdBuffer[j];
            if (drawCmd.UserCallback != nullptr)
            {
                auto callbackData = reinterpret_cast<const ImGuiCallbackData*>(drawCmd.UserCallbackData);

                switch (static_cast<ImGuiCallback>(reinterpret_cast<size_t>(drawCmd.UserCallback)))
                {
                case ImGuiCallback::SetGradient:
                    setPushConstants(&pushConstants.boundsMin, &callbackData->setGradient, sizeof(callbackData->setGradient));
                    break;       
                case ImGuiCallback::SetShaderModifier:
                    setPushConstants(&pushConstants.shaderModifier, &callbackData->setShaderModifier, sizeof(callbackData->setShaderModifier));
                    break;
                case ImGuiCallback::SetOrigin:
                    setPushConstants(&pushConstants.origin, &callbackData->setOrigin, sizeof(callbackData->setOrigin));
                    break;
                case ImGuiCallback::SetScale:
                    setPushConstants(&pushConstants.scale, &callbackData->setScale, sizeof(callbackData->setScale));
                    break;       
                case ImGuiCallback::SetMarqueeFade:
                    setPushConstants(&pushConstants.boundsMin, &callbackData->setMarqueeFade, sizeof(callbackData->setMarqueeFade));
                    break;
                case ImGuiCallback::SetOutline:
                    setPushConstants(&pushConstants.outline, &callbackData->setOutline, sizeof(callbackData->setOutline));
                    break;
                case ImGuiCallback::SetProceduralOrigin:
                    setPushConstants(&pushConstants.proceduralOrigin, &callbackData->setProceduralOrigin, sizeof(callbackData->setProceduralOrigin));
                    break;
                case ImGuiCallback::SetAdditive:
                {
                    auto pipelineToSet = callbackData->setAdditive.enabled ? g_imAdditivePipeline.get() : g_imPipeline.get();
                    if (pipeline != pipelineToSet)
                    {
                        commandList->setPipeline(pipelineToSet);
                        pipeline = pipelineToSet;
                    }
                    break;
                }
                default:
                    assert(false && "Unknown ImGui callback type.");
                    break;
                }
            }
            else
            {
                if (drawCmd.ClipRect.z <= drawCmd.ClipRect.x || drawCmd.ClipRect.w <= drawCmd.ClipRect.y)
                    continue;

                auto texture = reinterpret_cast<GuestTexture*>(drawCmd.TextureId);
                uint32_t descriptorIndex = TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D;
                if (texture != nullptr)
                {
                    if (texture->layout != RenderTextureLayout::SHADER_READ)
                    {
                        commandList->barriers(RenderBarrierStage::GRAPHICS | RenderBarrierStage::COPY,
                            RenderTextureBarrier(texture->texture, RenderTextureLayout::SHADER_READ));

                        texture->layout = RenderTextureLayout::SHADER_READ;
                    }

                    descriptorIndex = texture->descriptorIndex;

                    if (texture == ImFontAtlasSnapshot::FontTexture.get())
                        descriptorIndex |= 0x80000000;

                    setPushConstants(&pushConstants.texture2DDescriptorIndex, &descriptorIndex, sizeof(descriptorIndex));
                }

                if (pushConstantRangeMin < pushConstantRangeMax)
                {
                    commandList->setGraphicsPushConstants(0, reinterpret_cast<const uint8_t*>(&pushConstants) + pushConstantRangeMin, pushConstantRangeMin, pushConstantRangeMax - pushConstantRangeMin);
                    pushConstantRangeMin = ~0;
                    pushConstantRangeMax = 0;
                }

                if (memcmp(&clipRect, &drawCmd.ClipRect, sizeof(clipRect)) != 0)
                {
                    commandList->setScissors(RenderRect(int32_t(drawCmd.ClipRect.x), int32_t(drawCmd.ClipRect.y), int32_t(drawCmd.ClipRect.z), int32_t(drawCmd.ClipRect.w)));
                    clipRect = drawCmd.ClipRect;
                }

                commandList->drawIndexedInstanced(drawCmd.ElemCount, 1, drawCmd.IdxOffset, drawCmd.VtxOffset, 0);
            }
        }
    }
}

// We have to check for this to properly handle the following situation:
// 1. Wait on swap chain.
// 2. Create loading thread.
// 3. Loading thread also waits on swap chain.
// 4. Loading thread presents and quits.
// 5. After the loading thread quits, application also presents.
static bool g_pendingWaitOnSwapChain = true;

void Video::WaitOnSwapChain()
{
    if (g_pendingWaitOnSwapChain)
    {
        if (g_swapChainValid)
        {
            g_presentWaitProfiler.Begin();
            g_swapChain->wait();
            g_presentWaitProfiler.End();
        }

        g_pendingWaitOnSwapChain = false;
    }
}

static std::atomic<bool> g_executedCommandList;

void CreateTextureLocal(Sonicteam::SoX::Graphics::Texture* pTexture, uint32_t width, uint32_t height, uint32_t depth, uint32_t levels, uint32_t usage, uint32_t format, uint32_t pool, uint32_t type)
{
    auto pGuestTexture = (GuestTexture*)pTexture->m_pTexture.get();
    auto pGuestTextureNew = CreateTexture(width, height, depth, levels, usage, format, pool, type);

    if (pGuestTexture->sourceSurface != nullptr)
    {
        pGuestTexture->sourceSurface->destinationTextures.erase(pGuestTexture);
        pGuestTexture->sourceSurface = nullptr;
    }

    // Swap host state in place so guest references stay valid, then destroy the old state through the deferred path.
    std::swap(*pGuestTexture, *pGuestTextureNew);
    std::swap(pGuestTexture->refCount, pGuestTextureNew->refCount);
    DestructResource(pGuestTextureNew);

    // Update the texture dimensions
    pTexture->m_Width = width;
    pTexture->m_Height = height;
}

static std::vector<std::pair<stdx::string, boost::shared_ptr<Sonicteam::SoX::Engine::RenderProcess>>> g_renderProcessCache{};

static bool CacheRenderProcess(Sonicteam::SoX::Engine::RenderScheduler* pRenderScheduler, const char* pName)
{
    for (auto& rRenderProcess : pRenderScheduler->m_lRenderProcesses)
    {
        if (rRenderProcess.first != pName)
            continue;

        g_renderProcessCache.push_back(rRenderProcess);

        return true;
    }

    return false;
}

static bool FindRenderProcess(lua50::lua_State* L, const char* pName)
{
    const auto pCallbackData = reinterpret_cast<Sonicteam::RenderAction::CallbackData*>(lua50::lua_topointer(L, 1));

    auto it = std::find_if(g_renderProcessCache.begin(), g_renderProcessCache.end(), [&](const auto& rRenderProcess)
    {
        return rRenderProcess.first == pName;
    });

    if (it == g_renderProcessCache.end())
        return false;

    // Cache hit, push cached render process to scheduler.
    pCallbackData->pRenderScheduler->m_lRenderProcesses.push_back(*it);

    // Remove cached render process.
    g_renderProcessCache.erase(it);

    return true;
}

// RenderGE1Particle callback
PPC_FUNC_IMPL(__imp__sub_8260AAB0);
PPC_FUNC(sub_8260AAB0)
{
    const auto L = reinterpret_cast<lua50::lua_State*>(ctx.r3.u32 + base);

    if (FindRenderProcess(L, "GE1Particle"))
    {
        ctx.r3.u32 = 1;
    }
    else
    {
        __imp__sub_8260AAB0(ctx, base);
    }
}

// RenderSpanverse callback
PPC_FUNC_IMPL(__imp__sub_8260A9D0);
PPC_FUNC(sub_8260A9D0)
{
    const auto L = reinterpret_cast<lua50::lua_State*>(ctx.r3.u32 + base);

    if (FindRenderProcess(L, "Spanverse"))
    {
        ctx.r3.u32 = 1;
    }
    else 
    {
        __imp__sub_8260A9D0(ctx, base);
    }
}

void Video::Present() 
{
    g_renderQueue.enqueue(RenderCommands::ExecutePendingStretchRectCommands{});

    DrawImGui();

    g_renderQueue.enqueue(RenderCommands::ExecuteCommandList{});

    g_executedCommandList.wait(false);
    g_executedCommandList = false;

    if (g_swapChainValid)
    {
        if (g_pendingWaitOnSwapChain)
        {
            g_presentWaitProfiler.Begin();
            g_swapChain->wait(); // Never gonna happen outside loading threads as explained above.
            g_presentWaitProfiler.End();
        }

        RenderCommandSemaphore* signalSemaphores[] = { g_renderSemaphores[g_backBufferIndex].get() };
        g_swapChainValid = g_swapChain->present(g_backBufferIndex, signalSemaphores, std::size(signalSemaphores));
    }

    g_pendingWaitOnSwapChain = true;

    g_frame = g_nextFrame;
    g_nextFrame = (g_frame + 1) % NUM_FRAMES;

    if (g_commandListStates[g_frame])
    {
        g_frameFenceProfiler.Begin();
        g_queue->waitForCommandFence(g_commandFences[g_frame].get());
        g_frameFenceProfiler.End();
        g_commandListStates[g_frame] = false;

        // Update the GPU profiler with the results from the timestamps of the frame.
        g_queryPools[g_frame]->queryResults();
        const uint64_t *frameTimestamps = g_queryPools[g_frame]->getResults();
        g_gpuFrameProfiler.Set(double(frameTimestamps[1] - frameTimestamps[0]) / 1000000.0);
    }

    g_dirtyStates = DirtyStates(true);
    g_uploadAllocators[g_frame].reset();
    ResetIntermediaryUploadAllocator();
    g_triangleFanIndexData.reset();
    g_quadIndexData.reset();

    if (s_needsResize)
    {
        Resize(s_viewportWidth, s_viewportHeight);
        s_needsResize = false;
    }

    CheckSwapChain();

    g_renderQueue.enqueue(RenderCommands::BeginCommandList{});

    if (Config::FPS >= FPS_MIN && Config::FPS < FPS_MAX)
    {
        using namespace std::chrono_literals;

        static std::chrono::steady_clock::time_point s_next;

        auto now = std::chrono::steady_clock::now();

        if (now < s_next)
        {
            std::this_thread::sleep_for(std::chrono::floor<std::chrono::milliseconds>(s_next - now - 2ms));

            while ((now = std::chrono::steady_clock::now()) < s_next)
                std::this_thread::yield();
        }
        else
        {
            s_next = now;
        }

        s_next += 1000000000ns / Config::FPS;
    }

    g_presentProfiler.Reset();
}

void Video::Resize(uint32_t width, uint32_t height)
{
    const auto pApp = App::s_pApp;

    if (!pApp)
        return;

    const auto pDoc = pApp->m_pDoc.get();
    const auto pResourceManager = Sonicteam::SoX::ResourceManager::GetInstance();
    const auto pTextureMgr = Sonicteam::SoX::Graphics::TextureMgr::GetInstance();

    if (!pDoc || !pResourceManager || !pTextureMgr)
        return;

    const auto pRenderTargetContainer = pDoc->m_pRenderTargetContainer.get();
    const auto pMyGraphicsDevice = pDoc->m_pMyGraphicsDevice.get();

    if (!pRenderTargetContainer || !pMyGraphicsDevice)
        return;

    width *= Config::ResolutionScale;
    height *= Config::ResolutionScale;

    if (!width || !height)
    {
        LOGN_ERROR("Attempting to resize buffers to 0x0.");
        return;
    }

    struct BufferInfo
    {
        uint32_t Width;
        uint32_t Height;
        uint32_t Index;
        uint32_t Flags;
    };

    static std::map<std::string, BufferInfo> buffers{};

    // Update buffer dimensions.
    buffers["framebuffer0"] = { width, height, 0, 4 };
    buffers["framebuffer1"] = { width, height, 0, 0 };
    buffers["framebuffer_1_4_0"] = { width >> 2, height >> 2, 3, 2 };
    buffers["framebuffer_1_4_1"] = { width >> 2, height >> 2, 0, 2 };
    buffers["framebuffer_1_8_0"] = { width >> 3, height >> 3, 3, 2 };
    buffers["framebuffer_1_8_1"] = { width >> 3, height >> 3, 0, 2 };
    buffers["framebuffer_1_16_0"] = { width >> 4, height >> 4, 3, 2 };
    buffers["framebuffer_1_16_1"] = { width >> 4, height >> 4, 0, 2 };
    buffers["framebuffer_1_32_0"] = { width >> 5, height >> 5, 3, 2 };
    buffers["framebuffer_1_32_1"] = { width >> 5, height >> 5, 0, 2 };
    buffers["depthstencil_1_4"] = { width, height, 6, 0 };

    if (g_backBuffer && g_backBuffer != pApp->m_pBackBufferSurface.get())
        g_backBuffer->Release();

    if (g_depthStencil && g_depthStencil != pApp->m_pDepthStencilSurface.get())
        g_depthStencil->Release();

    auto& rDeviceInfo = pApp->m_DeviceInfo;

    auto surfaceParams = D3DXBSURFACE_PARAMETERS(0, 0, 0);
    const auto surfaceBase = uint32_t(height * 1.155555555555556); // 720p: 0x340

    // Recreate main buffers.
    ReleaseResource(static_cast<GuestTexture*>(pApp->m_pFrontBufferTexture.get()));
    pApp->m_pFrontBufferTexture = CreateTexture(width, height, 1, 1, 1, D3DFMT_LE_X8R8G8B8, 0, 3);
    ReleaseResource(static_cast<GuestSurface*>(pApp->m_pBackBufferSurface.get()));
    pApp->m_pBackBufferSurface = CreateSurface(width, height, D3DFMT_A8R8G8B8, 0, reinterpret_cast<GuestSurfaceCreateParams*>(&surfaceParams));

    rDeviceInfo.SurfaceParamsA = surfaceParams;
    surfaceParams.Base = surfaceParams.Base + surfaceBase;

    ReleaseResource(static_cast<GuestSurface*>(pApp->m_pDepthStencilSurface.get()));
    pApp->m_pDepthStencilSurface = CreateSurface(width, height, D3DFMT_D24FS8, 0, reinterpret_cast<GuestSurfaceCreateParams*>(&surfaceParams));

    rDeviceInfo.SurfaceParamsB = surfaceParams;
    rDeviceInfo.SurfaceParamsC = rDeviceInfo.SurfaceParamsB;
    rDeviceInfo.SurfaceParamsC.Base = rDeviceInfo.SurfaceParamsB.Base + surfaceBase;

    // Viewport is reset here because we're using the game's backbuffer directly.
    SetRenderTarget(static_cast<GuestDevice*>(pApp->m_pDevice.get()), 0, static_cast<GuestSurface*>(pApp->m_pBackBufferSurface.get()));
    SetDepthStencilSurface(static_cast<GuestDevice*>(pApp->m_pDevice.get()), static_cast<GuestSurface*>(pApp->m_pDepthStencilSurface.get()));

    g_backBuffer = static_cast<GuestSurface*>(pApp->m_pBackBufferSurface.get());
    g_depthStencil = static_cast<GuestSurface*>(pApp->m_pDepthStencilSurface.get());

    g_backBuffer->format = BACKBUFFER_FORMAT;

    rDeviceInfo.PresentParameters.BackBufferWidth = width;
    rDeviceInfo.PresentParameters.BackBufferHeight = height;
    rDeviceInfo.pColorTile2x = pApp->m_pColorTile2x;
    rDeviceInfo.pDepthTile2x = pApp->m_pDepthTile2x;
    rDeviceInfo.pColorTile4x = pApp->m_pColorTile4x;
    rDeviceInfo.pDepthTile4x = pApp->m_pDepthTile4x;

    pMyGraphicsDevice->m_SurfaceParamsA = rDeviceInfo.SurfaceParamsA;
    pMyGraphicsDevice->m_SurfaceParamsB = rDeviceInfo.SurfaceParamsB;
    pMyGraphicsDevice->m_SurfaceParamsC = rDeviceInfo.SurfaceParamsC;

    const auto setSurface = [&](Sonicteam::SoX::Graphics::Surface* pSurface, GuestSurface* pGuestSurface)
    {
        if (!pSurface)
            return;

        GuestToHostFunction<void>(sub_82593038, pSurface, pGuestSurface);
    };

    setSurface(pMyGraphicsDevice->m_spBackBuffer.get(), static_cast<GuestSurface*>(pApp->m_pBackBufferSurface.get()));
    setSurface(pMyGraphicsDevice->m_spDepthStencil.get(), static_cast<GuestSurface*>(pApp->m_pDepthStencilSurface.get()));

    // Refresh graphics device and configure FBO surfaces.
    GuestToHostFunction<void>(sub_82637418, pMyGraphicsDevice);
    GuestToHostFunction<void>(sub_825BAE48, pMyGraphicsDevice->m_spFrameBufferObject.get(), 0, &pMyGraphicsDevice->m_spBackBuffer);
    GuestToHostFunction<void>(sub_825BAEB8, pMyGraphicsDevice->m_spFrameBufferObject.get(), &pMyGraphicsDevice->m_spDepthStencil);

    struct FormatConfig
    {
        be<GuestFormat> SurfaceFormat;
        be<GuestFormat> TextureFormat;
        be<uint32_t> Usage;
    };

    const auto pFormatConfigs = reinterpret_cast<FormatConfig*>(g_memory.base + 0x82B7BD20);

    const auto getSurfaceParams = [&](uint32_t index, uint32_t flags) -> D3DXBSURFACE_PARAMETERS*
    {
        if (pFormatConfigs[index].Usage != 1 || (flags & 1) != 0)
            return &pMyGraphicsDevice->m_SurfaceParamsB;

        return (flags & 2) == 0
            ? &pMyGraphicsDevice->m_SurfaceParamsA
            : &pMyGraphicsDevice->m_SurfaceParamsC;
    };

    // Update depth stencil surfaces.
    for (auto& rSurface : pRenderTargetContainer->m_mspDepthStencil_1_4)
    {
        const auto pSurfaceName = rSurface.first.c_str();

        if (!buffers.contains(pSurfaceName))
            continue;

        const auto& rBufferInfo = buffers[pSurfaceName];

        auto pSurfaceParams = getSurfaceParams(rBufferInfo.Index, rBufferInfo.Flags);
        auto pNewSurface = CreateSurface(rBufferInfo.Width, rBufferInfo.Height, pFormatConfigs[rBufferInfo.Index].SurfaceFormat, 0, reinterpret_cast<GuestSurfaceCreateParams*>(pSurfaceParams));

        GuestToHostFunction<void>(sub_82592E98, rSurface.second.get(), pNewSurface, rBufferInfo.Width, rBufferInfo.Height);
    }

    // Update frame buffer textures.
    for (auto& rTexture : pRenderTargetContainer->m_mspFrameBuffer)
    {
        const auto pTextureName = rTexture.first.c_str();

        if (!buffers.contains(pTextureName))
            continue;

        const auto& rBufferInfo = buffers[pTextureName];

        CreateTextureLocal(rTexture.second.get(), rBufferInfo.Width, rBufferInfo.Height, 1, 1, pFormatConfigs[rBufferInfo.Index].Usage, pFormatConfigs[rBufferInfo.Index].TextureFormat, 0, 3);

        const auto surfaceFormatIndex = (rBufferInfo.Flags & 4) == 0
            ? rBufferInfo.Index
            : 3;

        auto pSurfaceParams = getSurfaceParams(surfaceFormatIndex, rBufferInfo.Flags);
        auto pNewSurface = CreateSurface(rBufferInfo.Width, rBufferInfo.Height, pFormatConfigs[surfaceFormatIndex].SurfaceFormat, 0, reinterpret_cast<GuestSurfaceCreateParams*>(pSurfaceParams));

        GuestToHostFunction<void>(sub_82592E98, rTexture.second->m_aspSurfaces[0].get(), pNewSurface, rBufferInfo.Width, rBufferInfo.Height);
    }

    // Clear post-process buffers.
    pRenderTargetContainer->m_mspDepthStencil_256.clear();
    pRenderTargetContainer->m_mspPostEffect.clear();
    pRenderTargetContainer->m_mspPostEffectAfter.clear();

    Sonicteam::HUDRaderMap* pHUDRaderMap{};

    auto& rTextureResources = pResourceManager->m_Resources[pTextureMgr->m_ID];

    if (const auto pGame = pApp->GetGame(); rTextureResources.find("radermap") != rTextureResources.end())
    {
        if (const auto pPopupScreenTask = pGame->m_rPopupScreenTask.m_pRef)
        {
            pHUDRaderMap = pPopupScreenTask->GetHUDPopupScreen<Sonicteam::HUDRaderMap>();

            // Release "radermap" textures.
            pHUDRaderMap->m_pMainTexture.reset();
            pHUDRaderMap->m_pMaskTexture.reset();
        }
    }

    const auto sfxMatrices1 = pDoc->m_pSFXAgent->m_aSFXMatrices1;
    const auto sfxMatrices2 = pDoc->m_pSFXAgent->m_aSFXMatrices2;
    pDoc->m_pSFXAgent->m_aSFXMatrices1 = 0;
    pDoc->m_pSFXAgent->m_aSFXMatrices2 = 0;

    // Cache particle render processes.
    CacheRenderProcess(pDoc->m_pRenderScheduler, "GE1Particle");
    CacheRenderProcess(pDoc->m_pRenderScheduler, "Spanverse");

    // Reload current render script.
    GuestToHostFunction<void>(sub_8260DF88, pDoc, Sonicteam::Globals::ms_pCurrentRenderScript, 1);

    // Drop cached render processes, no longer needed after reloading render script.
    g_renderProcessCache.clear();

    if (pDoc->m_pSFXAgent->m_aSFXMatrices1)
        g_userHeap.Free(pDoc->m_pSFXAgent->m_aSFXMatrices1->GetArray());

    if (pDoc->m_pSFXAgent->m_aSFXMatrices2)
        g_userHeap.Free(pDoc->m_pSFXAgent->m_aSFXMatrices2->GetArray());

    pDoc->m_pSFXAgent->m_aSFXMatrices1 = sfxMatrices1;
    pDoc->m_pSFXAgent->m_aSFXMatrices2 = sfxMatrices2;

    if (pHUDRaderMap)
    {
        const auto setTexture = [&](const char* pName, auto* spTexture)
        {
            if (auto it = rTextureResources.find(pName); it != rTextureResources.end())
                *spTexture = reinterpret_cast<Sonicteam::MyTexture*>(it->second.get());
        };

        // Set new "radermap" textures after Lua script reload.
        setTexture("radermap", &pHUDRaderMap->m_pMainTexture);
        setTexture("radermap_mask", &pHUDRaderMap->m_pMaskTexture);
    }
}

void Video::StartPipelinePrecompilation()
{
    LoadingPatches::Events.push_back(&g_loadingPipelineCompilationEvent);

    EnqueuePipelineTask(PipelineTaskType::PrecompilePipelines);
}

static void SetRootDescriptor(const UploadAllocation& allocation, size_t index)
{
    auto& commandList = g_commandLists[g_frame];

    if (g_backend != Backend::D3D12)
        commandList->setGraphicsPushConstants(0, &allocation.deviceAddress, 8 * index, 8);
    else
        commandList->setGraphicsRootDescriptor(allocation.buffer->at(allocation.offset), index);
}

static void ProcRenderCommand(const RenderCommands::ExecuteCommandList& cmd)
{    
    if (g_swapChainValid)
    {
        auto swapChainTexture = g_swapChain->getTexture(g_backBufferIndex);
        if (g_backBuffer->texture == g_intermediaryBackBufferTexture.get())
        {
            struct
            {
                float gamma;
                uint32_t textureDescriptorIndex;

                int32_t viewportOffsetX;
                int32_t viewportOffsetY;
                int32_t viewportWidth;
                int32_t viewportHeight;
            } constants;

            constants.gamma = 0.85f;

            float offset = (Config::Brightness - 0.5f) * 1.2f;

            constants.gamma = 1.0f / std::clamp(constants.gamma + offset, 0.1f, 4.0f);
            constants.textureDescriptorIndex = g_intermediaryBackBufferTextureDescriptorIndex;

            constants.viewportOffsetX = (int32_t(g_swapChain->getWidth()) - int32_t(Video::s_viewportWidth)) / 2;
            constants.viewportOffsetY = (int32_t(g_swapChain->getHeight()) - int32_t(Video::s_viewportHeight)) / 2;
            constants.viewportWidth = Video::s_viewportWidth;
            constants.viewportHeight = Video::s_viewportHeight;

            auto &framebuffer = g_backBuffer->framebuffers[swapChainTexture];
            if (!framebuffer)
            {
                RenderFramebufferDesc desc;
                desc.colorAttachments = const_cast<const RenderTexture **>(&swapChainTexture);
                desc.colorAttachmentsCount = 1;
                framebuffer = g_device->createFramebuffer(desc);
            }

            RenderTextureBarrier srcBarriers[] =
            {
                RenderTextureBarrier(g_intermediaryBackBufferTexture.get(), RenderTextureLayout::SHADER_READ),
                RenderTextureBarrier(swapChainTexture, RenderTextureLayout::COLOR_WRITE)
            };

            auto &commandList = g_commandLists[g_frame];
            commandList->barriers(RenderBarrierStage::GRAPHICS, srcBarriers, std::size(srcBarriers));
            commandList->setGraphicsPipelineLayout(g_pipelineLayout.get());
            commandList->setPipeline(g_gammaCorrectionPipeline.get());
            commandList->setGraphicsDescriptorSet(g_textureDescriptorSet.get(), 0);
            SetRootDescriptor(g_uploadAllocators[g_frame].allocate<false>(&constants, sizeof(constants), 0x100), 2);
            commandList->setFramebuffer(framebuffer.get());
            commandList->setViewports(RenderViewport(0.0f, 0.0f, g_swapChain->getWidth(), g_swapChain->getHeight()));
            commandList->setScissors(RenderRect(0, 0, g_swapChain->getWidth(), g_swapChain->getHeight()));
            commandList->drawInstanced(6, 1, 0, 0);
            commandList->barriers(RenderBarrierStage::GRAPHICS, RenderTextureBarrier(swapChainTexture, RenderTextureLayout::PRESENT));
        }
        else
        {
            AddBarrier(g_backBuffer, RenderTextureLayout::PRESENT);
            FlushBarriers();
        }
    }

    auto &commandList = g_commandLists[g_frame];
    commandList->writeTimestamp(g_queryPools[g_frame].get(), 1);
    commandList->end();

    if (g_swapChainValid)
    {
        const RenderCommandList *commandLists[] = { commandList.get() };
        RenderCommandSemaphore *waitSemaphores[] = { g_acquireSemaphores[g_frame].get() };
        RenderCommandSemaphore *signalSemaphores[] = { g_renderSemaphores[g_backBufferIndex].get() };

        g_queue->executeCommandLists(
            commandLists, std::size(commandLists),
            waitSemaphores, std::size(waitSemaphores),
            signalSemaphores, std::size(signalSemaphores),
            g_commandFences[g_frame].get());
    }
    else
    {
        g_queue->executeCommandLists(commandList.get(), g_commandFences[g_frame].get());
    }

    g_commandListStates[g_frame] = true;

    g_executedCommandList = true;
    g_executedCommandList.notify_one();
}

static void ProcRenderCommand(const RenderCommands::BeginCommandList& cmd)
{
    g_renderFrame = g_frame;
    DestructTempResources();
    BeginCommandList();
}

static GuestSurface* GetBackBuffer()
{
    if (g_backBuffer)
        g_backBuffer->AddRef();

    return g_backBuffer;
}

static GuestSurface* GetDepthStencil() 
{
    if (g_depthStencil)
        g_depthStencil->AddRef();

    return g_depthStencil;
}

void Video::ComputeViewportDimensions()
{
    uint32_t width = g_swapChain->getWidth();
    uint32_t height = g_swapChain->getHeight();
    float aspectRatio = float(width) / float(height);

    switch (Config::AspectRatio)
    {
        case EAspectRatio::Original:
        {
            if (aspectRatio > WIDE_ASPECT_RATIO)
            {
                s_viewportWidth = height * 16 / 9;
                s_viewportHeight = height;
            }
            else
            {
                s_viewportWidth = width;
                s_viewportHeight = width * 9 / 16;
            }

            break;
        }

        default:
            s_viewportWidth = width;
            s_viewportHeight = height;
            break;
    }

    AspectRatioPatches::ComputeOffsets();
}

void DiscardTexture(GuestBaseTexture* texture, RenderTextureLayout layout)
{
    if (g_backend == Backend::D3D12)
    {
        std::lock_guard lock(g_discardMutex);

        g_discardCommandList->begin();
        auto& layoutRef = GetLayoutRef(texture);
        if (layoutRef != layout)
        {
            g_discardCommandList->barriers(RenderBarrierStage::GRAPHICS, RenderTextureBarrier(texture->texture, layout));
            layoutRef = layout;
        }

        g_discardCommandList->discardTexture(texture->texture);
        g_discardCommandList->end();

        g_queue->executeCommandLists(g_discardCommandList.get(), g_discardCommandFence.get());
        g_queue->waitForCommandFence(g_discardCommandFence.get());
    }
}

static void FlushViewport()
{
    auto& commandList = g_commandLists[g_frame];

    if (g_dirtyStates.viewport)
    {
        auto viewport = g_viewport;

        // if (viewport.minDepth > viewport.maxDepth)
        //     std::swap(viewport.minDepth, viewport.maxDepth);

        commandList->setViewports(viewport);

        g_dirtyStates.viewport = false;
    }

    if (g_dirtyStates.scissorRect)
    {
        auto scissorRect = g_scissorTestEnable ? g_scissorRect : RenderRect(
            g_viewport.x,
            g_viewport.y,
            g_viewport.x + g_viewport.width,
            g_viewport.y + g_viewport.height);

        commandList->setScissors(scissorRect);

        g_dirtyStates.scissorRect = false;
    }
}

static void SetTextureInRenderThread(uint32_t index, GuestTexture* texture);
static void SetSurface(uint32_t index, GuestSurface* surface);

static void ProcRenderCommand(const RenderCommands::StretchRect& args)
{
    const bool isDepthStencil = (args.flags & 0x4) != 0;
    const auto surface = isDepthStencil ? g_depthStencil : g_renderTarget;

    // Erase previous pending command so it doesn't cause the texture to be overriden.
    if (args.texture->sourceSurface != nullptr)
        args.texture->sourceSurface->destinationTextures.erase(args.texture);

    args.texture->sourceSurface = surface;

    surface->destinationTextures.emplace(args.texture, args.destSliceOrFace);

    // If the texture is assigned to any slots, set it again. This'll also push the barrier.
    for (uint32_t i = 0; i < std::size(g_textures); i++)
    {
        if (g_textures[i] == args.texture)
        {
            // TODO: Render depth directly to slice and avoid copy
            // Set the original texture for MSAA and surface-to-array textures as they always get resolved.
            if (surface->sampleCount != RenderSampleCount::COUNT_1 ||
                args.texture->type == ResourceType::ArrayTexture)
            {
                SetTextureInRenderThread(i, args.texture);
                g_pendingResolves.emplace(surface);
            }
            else
            {
                SetSurface(i, surface);
            }
        }
    }

    // Remember to clear later.
    g_pendingSurfaceCopies.emplace(surface);
}

static void ProcRenderCommand(const RenderCommands::SetRenderTarget& args)
{
    SetDirtyValue(g_dirtyStates.renderTargetAndDepthStencil, g_renderTarget, args.renderTarget);
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.renderTargetFormat, args.renderTarget != nullptr ? args.renderTarget->format : RenderFormat::UNKNOWN);
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.sampleCount, args.renderTarget != nullptr ? args.renderTarget->sampleCount : RenderSampleCount::COUNT_1);

    // When alpha to coverage is enabled, update the alpha test mode as it's dependent on sample count.
    SetAlphaTestMode((g_pipelineState.specConstants & (SPEC_CONSTANT_ALPHA_TEST | SPEC_CONSTANT_ALPHA_TO_COVERAGE)) != 0);
}

static void ProcRenderCommand(const RenderCommands::SetDepthStencilSurface& args)
{
    SetDirtyValue(g_dirtyStates.renderTargetAndDepthStencil, g_depthStencil, args.depthStencil);
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.depthStencilFormat, args.depthStencil != nullptr ? args.depthStencil->format : RenderFormat::UNKNOWN);
}

static bool PopulateBarriersForStretchRect(GuestSurface* renderTarget, GuestSurface* depthStencil)
{
    bool addedAny = false;

    for (const auto surface : { renderTarget, depthStencil })
    {
        if (surface != nullptr && !surface->destinationTextures.empty())
        {
            const bool multiSampling = surface->sampleCount != RenderSampleCount::COUNT_1;

            RenderTextureLayout srcLayout;
            RenderTextureLayout dstLayout;
            bool shaderResolve = true;

            if (multiSampling)
            {
                if (!RenderFormatIsDepth(surface->format) || g_capabilities.resolveModes)
                {
                    srcLayout = RenderTextureLayout::RESOLVE_SOURCE;
                    dstLayout = RenderTextureLayout::RESOLVE_DEST;
                    shaderResolve = false;
                }
            }

            if (shaderResolve)
            {
                srcLayout = RenderTextureLayout::SHADER_READ;
                dstLayout = (RenderFormatIsDepth(surface->format) ? RenderTextureLayout::DEPTH_WRITE : RenderTextureLayout::COLOR_WRITE);
            }

            AddBarrier(surface, srcLayout);

            for (const auto [texture, _] : surface->destinationTextures)
                AddBarrier(texture, dstLayout);

            addedAny = true;
        }
    }

    return addedAny;
}

static void ExecutePendingStretchRectCommands(GuestSurface* renderTarget, GuestSurface* depthStencil)
{
    auto& commandList = g_commandLists[g_frame];

    for (const auto surface : { renderTarget, depthStencil })
    {
        if (surface != nullptr && !surface->destinationTextures.empty())
        {
            const bool multiSampling = surface->sampleCount != RenderSampleCount::COUNT_1;
            const bool isDepthStencil = RenderFormatIsDepth(surface->format);

            for (const auto [texture, slice] : surface->destinationTextures)
            {
                bool shaderResolve = true;

                if (multiSampling)
                {
                    if (!isDepthStencil || g_capabilities.resolveModes)
                    {
                        if (isDepthStencil)
                            commandList->resolveTextureRegion(texture->texture, 0, 0, surface->texture, nullptr, RenderResolveMode::MIN);
                        else
                            commandList->resolveTexture(texture->texture, surface->texture);

                        shaderResolve = false;
                    }
                }

                if (shaderResolve)
                {
                    RenderPipeline* pipeline = nullptr;

                    if (multiSampling)
                    {
                        uint32_t pipelineIndex = 0;

                        switch (surface->sampleCount)
                        {
                        case RenderSampleCount::COUNT_2:
                            pipelineIndex = 0;
                            break;
                        case RenderSampleCount::COUNT_4:
                            pipelineIndex = 1;
                            break;
                        case RenderSampleCount::COUNT_8:
                            pipelineIndex = 2;
                            break;
                        default:
                            assert(false && "Unsupported MSAA sample count");
                            break;
                        }

                        if (isDepthStencil)
                        {
                            pipeline = g_resolveMsaaDepthPipelines[pipelineIndex].get();
                        }
                        else
                        {
                            auto& resolveMsaaColorPipeline = g_resolveMsaaColorPipelines[surface->format][pipelineIndex];
                            if (resolveMsaaColorPipeline == nullptr)
                            {
                                RenderGraphicsPipelineDesc desc;
                                desc.pipelineLayout = g_pipelineLayout.get();
                                desc.vertexShader = g_copyShader.get();
                                desc.pixelShader = g_resolveMsaaColorShaders[pipelineIndex].get();
                                desc.renderTargetFormat[0] = texture->format;
                                desc.renderTargetBlend[0] = RenderBlendDesc::Copy();
                                desc.renderTargetCount = 1;
                                resolveMsaaColorPipeline = g_device->createGraphicsPipeline(desc);
                            }

                            pipeline = resolveMsaaColorPipeline.get();
                        }
                    }
                    else
                    {
                        if (isDepthStencil)
                        {
                            pipeline = g_copyDepthPipeline.get();
                        }
                        else
                        {
                            auto& copyColorPipeline = g_copyColorPipelines[texture->format];
                            if (copyColorPipeline == nullptr)
                            {
                                RenderGraphicsPipelineDesc desc;
                                desc.pipelineLayout = g_pipelineLayout.get();
                                desc.vertexShader = g_copyShader.get();
                                desc.pixelShader = g_copyColorShader.get();
                                desc.renderTargetFormat[0] = texture->format;
                                desc.renderTargetBlend[0] = RenderBlendDesc::Copy();
                                desc.renderTargetCount = 1;
                                copyColorPipeline = g_device->createGraphicsPipeline(desc);
                            }

                            pipeline = copyColorPipeline.get();
                        }
                    }

                    auto& framebuffer = texture->framebuffers[slice];
                    if (framebuffer == nullptr)
                    {
                        if (isDepthStencil)
                        {
                            RenderTextureViewDesc viewDesc;
                            viewDesc.format = texture->format;
                            viewDesc.dimension = texture->viewDimension;
                            viewDesc.mipLevels = texture->mipLevels;
                            viewDesc.arrayIndex = slice;
                            viewDesc.arraySize = 1;
                            auto& view = texture->framebufferViews.emplace_back(texture->texture->createTextureView(viewDesc));

                            RenderFramebufferDesc desc;
                            desc.depthAttachmentView = view.get();
                            framebuffer = g_device->createFramebuffer(desc);
                        }
                        else
                        {
                            RenderFramebufferDesc desc;
                            desc.colorAttachments = const_cast<const RenderTexture**>(&texture->texture);
                            desc.colorAttachmentsCount = 1;
                            framebuffer = g_device->createFramebuffer(desc);
                        }
                    }

                    if (g_framebuffer != framebuffer.get())
                    {
                        commandList->setFramebuffer(framebuffer.get());
                        g_framebuffer = framebuffer.get();
                    }

                    commandList->setPipeline(pipeline);
                    commandList->setViewports(RenderViewport(0.0f, 0.0f, float(texture->width), float(texture->height), 0.0f, 1.0f));
                    commandList->setScissors(RenderRect(0, 0, texture->width, texture->height));
                    commandList->setGraphicsPushConstants(0, &surface->descriptorIndex, 0, sizeof(uint32_t));
                    commandList->drawInstanced(6, 1, 0, 0);

                    g_dirtyStates.renderTargetAndDepthStencil = true;
                    g_dirtyStates.viewport = true;
                    g_dirtyStates.pipelineState = true;
                    g_dirtyStates.scissorRect = true;

                    if (g_backend != Backend::D3D12)
                    {
                        g_dirtyStates.vertexShaderConstants = true; // The push constant call invalidates vertex shader constants.
                        g_dirtyStates.depthBias = true; // Static depth bias in copy pipeline invalidates dynamic depth bias.
                    }
                }

                texture->sourceSurface = nullptr;

                // Check if any texture slots had this texture assigned, and make it point back at the original texture.
                for (uint32_t i = 0; i < std::size(g_textures); i++)
                {
                    if (g_textures[i] == texture)
                        SetTextureInRenderThread(i, texture);
                }
            }

            surface->destinationTextures.clear();
        }
    }
}

static void ProcRenderCommand(const RenderCommands::ExecutePendingStretchRectCommands& args)
{
    bool foundAny = false;

    for (const auto surface : g_pendingSurfaceCopies)
    {
        // Depth stencil textures in this game are guaranteed to be transient.
        if (!RenderFormatIsDepth(surface->format))
            foundAny |= PopulateBarriersForStretchRect(surface, nullptr);
    }

    if (foundAny)
    {
        FlushBarriers();

        for (const auto surface : g_pendingSurfaceCopies)
        {
            if (!RenderFormatIsDepth(surface->format))
                ExecutePendingStretchRectCommands(surface, nullptr);

            for (const auto [texture, _] : surface->destinationTextures)
                texture->sourceSurface = nullptr;

            surface->destinationTextures.clear();
        }
    }

    g_pendingSurfaceCopies.clear();
    g_pendingResolves.clear();
}

static void SetFramebuffer(GuestSurface* renderTarget, GuestSurface* depthStencil, bool settingForClear)
{
    if (settingForClear || g_dirtyStates.renderTargetAndDepthStencil)
    {
        // printf("SetFramebuffer %x %x\n", renderTarget, depthStencil);
        GuestSurface* framebufferContainer = nullptr;
        RenderTexture* framebufferKey = nullptr;

        if (renderTarget != nullptr && depthStencil != nullptr)
        {
            framebufferContainer = depthStencil; // Backbuffer texture changes per frame so we can't use the depth stencil as the key.
            framebufferKey = renderTarget->texture;
        }
        else if (renderTarget != nullptr && depthStencil == nullptr)
        {
            framebufferContainer = renderTarget;
            framebufferKey = renderTarget->texture; // Backbuffer texture changes per frame so we can't assume nullptr for it.
        }
        else if (renderTarget == nullptr && depthStencil != nullptr)
        {
            framebufferContainer = depthStencil;
            framebufferKey = nullptr;
        }

        auto& commandList = g_commandLists[g_frame];

        if (framebufferContainer != nullptr)
        {
            auto& framebuffers = framebufferContainer->variant != nullptr ?
                framebufferContainer->variant->framebuffers : framebufferContainer->framebuffers;

            auto& framebuffer = framebuffers[framebufferKey];

            if (framebuffer == nullptr)
            {
                RenderFramebufferDesc desc;

                if (renderTarget != nullptr)
                {
                    desc.colorAttachments = const_cast<const RenderTexture**>(&renderTarget->texture);
                    desc.colorAttachmentsCount = 1;
                }

                if (depthStencil != nullptr)
                    desc.depthAttachment = depthStencil->texture;

                framebuffer = g_device->createFramebuffer(desc);
            }

            if (g_framebuffer != framebuffer.get())
            {
                commandList->setFramebuffer(framebuffer.get());
                g_framebuffer = framebuffer.get();
            }
        }
        else if (g_framebuffer != nullptr)
        {
            commandList->setFramebuffer(nullptr);
            g_framebuffer = nullptr;
        }

        if (g_framebuffer != nullptr)
        {
            SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.halfPixelOffsetX, 1.0f / float(g_framebuffer->getWidth()));
            SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.halfPixelOffsetY, -1.0f / float(g_framebuffer->getHeight()));
        }

        g_dirtyStates.renderTargetAndDepthStencil = settingForClear;
    }
}

static bool SharesStorage(const GuestSurface* lhs, const GuestSurface* rhs)
{
    return lhs != nullptr && rhs != nullptr && lhs->texture == rhs->texture;
}

static void FlushPendingCopiesForAliasedStorage(GuestSurface* renderTarget, GuestSurface* depthStencil)
{
    bool foundAny = false;

    for (const auto surface : g_pendingSurfaceCopies)
    {
        if (SharesStorage(surface, renderTarget) || SharesStorage(surface, depthStencil))
        {
            const bool isDepthStencil = RenderFormatIsDepth(surface->format);
            foundAny |= PopulateBarriersForStretchRect(isDepthStencil ? nullptr : surface, isDepthStencil ? surface : nullptr);
        }
    }

    if (foundAny)
    {
        FlushBarriers();

        for (const auto surface : g_pendingSurfaceCopies)
        {
            if (SharesStorage(surface, renderTarget) || SharesStorage(surface, depthStencil))
            {
                const bool isDepthStencil = RenderFormatIsDepth(surface->format);
                ExecutePendingStretchRectCommands(isDepthStencil ? nullptr : surface, isDepthStencil ? surface : nullptr);
            }
        }
    }
}

static void ProcRenderCommand(const RenderCommands::Clear& args)
{
    FlushPendingCopiesForAliasedStorage(g_renderTarget, g_depthStencil);

    AddBarrier(g_renderTarget, RenderTextureLayout::COLOR_WRITE);
    AddBarrier(g_depthStencil, RenderTextureLayout::DEPTH_WRITE);
    FlushBarriers();

    bool canClearInOnePass = (g_renderTarget == nullptr) || (g_depthStencil == nullptr) ||
        (g_renderTarget->width == g_depthStencil->width && g_renderTarget->height == g_depthStencil->height);

    if (canClearInOnePass)
    {
        SetFramebuffer(g_renderTarget, g_depthStencil, true);
    }

    auto& commandList = g_commandLists[g_frame];

    bool clearColor = (args.flags & D3DCLEAR_TARGET) != 0;
    bool clearDepth = (args.flags & D3DCLEAR_ZBUFFER) != 0;
    bool clearStencil = (args.flags & D3DCLEAR_STENCIL) != 0;

    // Try to propogate clears to aliased texture variants.
    // Note, this contains a bit of a hack to avoid the main depth buf
    // getting cleared to a value that makes every pixel fail depth test.
    if (g_renderTarget != nullptr && g_renderTarget->variant != nullptr &&
        (g_renderTarget->variant->deferredClearFlags & D3DCLEAR_TARGET) != 0)
    {
        clearColor = true;
        g_renderTarget->variant->deferredClearFlags &= ~D3DCLEAR_TARGET;
    }

    if (g_depthStencil != nullptr && g_depthStencil->variant != nullptr &&
        (g_depthStencil->variant->deferredClearFlags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)) != 0)
    {
        clearDepth |= (g_depthStencil->variant->deferredClearFlags & D3DCLEAR_ZBUFFER) != 0;
        clearStencil |= (g_depthStencil->variant->deferredClearFlags & D3DCLEAR_STENCIL) != 0;
        g_depthStencil->variant->deferredClearFlags &= ~(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
    }

    if (g_renderTarget != nullptr && clearColor)
    {
        if (!canClearInOnePass) {
            SetFramebuffer(g_renderTarget, nullptr, true);
        }

        commandList->clearColor(0, RenderColor(args.color[0], args.color[1], args.color[2], args.color[3]));
    }

    if (g_depthStencil != nullptr && (clearDepth || clearStencil))
    {
        if (!canClearInOnePass) {
            SetFramebuffer(nullptr, g_depthStencil, true);
        }

        commandList->clearDepthStencil(clearDepth, clearStencil, args.z, args.stencil);
    }

    PropagateDeferredClear(g_renderTarget, g_depthStencil, args);
}

static void ApplyDeferredVariantClears(GuestSurface* renderTarget, GuestSurface* depthStencil)
{
    auto& commandList = g_commandLists[g_frame];

    if (renderTarget != nullptr && renderTarget->variant != nullptr &&
        (renderTarget->variant->deferredClearFlags & D3DCLEAR_TARGET) != 0)
    {
        auto variant = renderTarget->variant;

        commandList->clearColor(0, RenderColor(variant->deferredClearColor[0], variant->deferredClearColor[1],
            variant->deferredClearColor[2], variant->deferredClearColor[3]));

        variant->deferredClearFlags &= ~D3DCLEAR_TARGET;
    }

    if (depthStencil != nullptr && depthStencil->variant != nullptr &&
        (depthStencil->variant->deferredClearFlags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)) != 0)
    {
        auto variant = depthStencil->variant;
        const bool clearDepth = (variant->deferredClearFlags & D3DCLEAR_ZBUFFER) != 0;
        const bool clearStencil = (variant->deferredClearFlags & D3DCLEAR_STENCIL) != 0;

        commandList->clearDepthStencil(clearDepth, clearStencil, variant->deferredClearZ, variant->deferredClearStencil);

        variant->deferredClearFlags &= ~(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
    }
}

static void ProcRenderCommand(const RenderCommands::SetViewport& args)
{
    SetDirtyValue<float>(g_dirtyStates.viewport, g_viewport.x, args.x);
    SetDirtyValue<float>(g_dirtyStates.viewport, g_viewport.y, args.y);
    SetDirtyValue<float>(g_dirtyStates.viewport, g_viewport.width, args.width);
    SetDirtyValue<float>(g_dirtyStates.viewport, g_viewport.height, args.height);
    SetDirtyValue<float>(g_dirtyStates.viewport, g_viewport.minDepth, args.minDepth);
    SetDirtyValue<float>(g_dirtyStates.viewport, g_viewport.maxDepth, args.maxDepth);
    
    uint32_t specConstants = g_pipelineState.specConstants;
    if (args.minDepth > args.maxDepth)
        specConstants |= SPEC_CONSTANT_REVERSE_Z;
    else 
        specConstants &= ~SPEC_CONSTANT_REVERSE_Z;

    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.specConstants, specConstants);

    g_dirtyStates.scissorRect |= g_dirtyStates.viewport;
}

static void SetTextureInRenderThread(uint32_t index, GuestTexture* texture)
{
    AddBarrier(texture, RenderTextureLayout::SHADER_READ);

    auto viewDimension = texture != nullptr ? texture->viewDimension : RenderTextureViewDimension::UNKNOWN;

    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.texture2DIndices[index],
        viewDimension == RenderTextureViewDimension::TEXTURE_2D ? texture->descriptorIndex : TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D);

    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.texture2DArrayIndices[index], texture != nullptr &&
        viewDimension == RenderTextureViewDimension::TEXTURE_2D ? texture->descriptorIndex : TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D_ARRAY);

    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.textureCubeIndices[index], texture != nullptr &&
        viewDimension == RenderTextureViewDimension::TEXTURE_CUBE ? texture->descriptorIndex : TEXTURE_DESCRIPTOR_NULL_TEXTURE_CUBE);
}

static void SetSurface(uint32_t index, GuestSurface* surface)
{
    AddBarrier(surface, RenderTextureLayout::SHADER_READ);

    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.texture2DIndices[index], surface->descriptorIndex);
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.texture2DArrayIndices[index], uint32_t(TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D_ARRAY));
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.textureCubeIndices[index], uint32_t(TEXTURE_DESCRIPTOR_NULL_TEXTURE_CUBE));
}

static void ProcRenderCommand(const RenderCommands::SetTexture& args)
{
    // If a pending copy operation is detected, set the source surface. The indices will be fixed later if flushing is necessary.
    bool shouldSetTexture = true;
    if (args.texture != nullptr && args.texture->sourceSurface != nullptr)
    {
        // TODO: Render depth directly to slice and avoid copy
        // MSAA surfaces or surface-to-array need to be resolved and cannot be used directly.
        if (args.texture->sourceSurface->sampleCount != RenderSampleCount::COUNT_1 ||
            args.texture->type == ResourceType::ArrayTexture)
        {
            g_pendingResolves.emplace(args.texture->sourceSurface);
        }
        else
        {
            SetSurface(args.index, args.texture->sourceSurface);
            shouldSetTexture = false;
        }
    }
    
    if (shouldSetTexture)
        SetTextureInRenderThread(args.index, args.texture);
    
    g_textures[args.index] = args.texture;
}

static void ProcRenderCommand(const RenderCommands::SetScissorRect& args)
{
    SetDirtyValue<int32_t>(g_dirtyStates.scissorRect, g_scissorRect.top, args.top);
    SetDirtyValue<int32_t>(g_dirtyStates.scissorRect, g_scissorRect.left, args.left);
    SetDirtyValue<int32_t>(g_dirtyStates.scissorRect, g_scissorRect.bottom, args.bottom);
    SetDirtyValue<int32_t>(g_dirtyStates.scissorRect, g_scissorRect.right, args.right);
}

static RenderShader* GetReplacementShader(GuestShader* guestShader)
{
    if (guestShader == nullptr)
        return nullptr;

    if (guestShader->shaderCacheEntry != nullptr)
    {
        switch (guestShader->shaderCacheEntry->hash)
        {
        case 0x3687D038CE7D0BEA:
        case 0xB4DA7A442DBB16CC:
            if (Config::RadialBlur == ERadialBlur::Enhanced)
                return g_enhancedBurnoutBlurVSShader.get();
            break;
        case 0xDA58F0110A8595D9:
        case 0x845A4EF989446C01:
            if (Config::RadialBlur == ERadialBlur::Enhanced)
                return g_enhancedBurnoutBlurPSShader.get();
            break;
        case 0x88EB0AA82E4257B0:
            return g_MoviePSShader.get();
        default:
            break;
        }
    }

    return nullptr;
}

static RenderShader* GetOrLinkShader(GuestShader* guestShader, uint32_t specConstants)
{
    auto* replacementShader = GetReplacementShader(guestShader);
    if (replacementShader != nullptr)
        return replacementShader;

    if (g_backend != Backend::D3D12 ||
        guestShader->shaderCacheEntry == nullptr ||
        guestShader->shaderCacheEntry->specConstantsMask == 0)
    {
        std::lock_guard lock(guestShader->mutex);

        if (guestShader->shader == nullptr)
        {
            assert(guestShader->shaderCacheEntry != nullptr);

            switch (g_backend) {
            case Backend::VULKAN:
            {
                auto compressedSpirvData = g_shaderCache.get() + guestShader->shaderCacheEntry->spirvOffset;

                std::vector<uint8_t> decoded(smolv::GetDecodedBufferSize(compressedSpirvData, guestShader->shaderCacheEntry->spirvSize));
                bool result = smolv::Decode(compressedSpirvData, guestShader->shaderCacheEntry->spirvSize, decoded.data(), decoded.size());
                assert(result);

                guestShader->shader = g_device->createShader(decoded.data(), decoded.size(), "shaderMain", RenderShaderFormat::SPIRV);
                break;
            }
            case Backend::D3D12:
            {
                guestShader->shader = g_device->createShader(g_shaderCache.get() + guestShader->shaderCacheEntry->dxilOffset,
                    guestShader->shaderCacheEntry->dxilSize, "shaderMain", RenderShaderFormat::DXIL);
                break;
            }
            case Backend::METAL:
            {
                guestShader->shader = g_device->createShader(g_shaderCache.get() + guestShader->shaderCacheEntry->airOffset,
                    guestShader->shaderCacheEntry->airSize, "shaderMain", RenderShaderFormat::METAL);
                break;
            }
            }

#ifdef _DEBUG
            guestShader->shader->setName(fmt::format("{}:{:x}", guestShader->shaderCacheEntry->filename, guestShader->shaderCacheEntry->hash));
#endif
        }

        return guestShader->shader.get();
    }

    specConstants &= guestShader->shaderCacheEntry->specConstantsMask;

    RenderShader* shader;
    {
        std::lock_guard lock(guestShader->mutex);
        shader = guestShader->linkedShaders[specConstants].get();
    }

#ifdef MARATHON_RECOMP_D3D12
    if (shader == nullptr)
    {
        static Mutex g_compiledSpecConstantLibraryBlobMutex;
        static ankerl::unordered_dense::map<uint32_t, ComPtr<IDxcBlob>> g_compiledSpecConstantLibraryBlobs;

        thread_local ComPtr<IDxcCompiler3> s_dxcCompiler;
        thread_local ComPtr<IDxcLinker> s_dxcLinker;
        thread_local ComPtr<IDxcUtils> s_dxcUtils;

        wchar_t specConstantsLibName[0x100];
        swprintf_s(specConstantsLibName, L"SpecConstants_%d", specConstants);

        ComPtr<IDxcBlob> specConstantLibraryBlob;
        {
            std::lock_guard lock(g_compiledSpecConstantLibraryBlobMutex);
            specConstantLibraryBlob = g_compiledSpecConstantLibraryBlobs[specConstants];
        }

        if (specConstantLibraryBlob == nullptr)
        {
            if (s_dxcCompiler == nullptr)
            {
                HRESULT hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(s_dxcCompiler.GetAddressOf()));
                assert(SUCCEEDED(hr) && s_dxcCompiler != nullptr);
            }

            char libraryHlsl[0x100];
            sprintf_s(libraryHlsl, "export uint g_SpecConstants() { return %d; }", specConstants);

            DxcBuffer buffer{};
            buffer.Ptr = libraryHlsl;
            buffer.Size = strlen(libraryHlsl);

            const wchar_t* args[1];
            args[0] = L"-T lib_6_3";

            ComPtr<IDxcResult> result;
            HRESULT hr = s_dxcCompiler->Compile(&buffer, args, std::size(args), nullptr, IID_PPV_ARGS(result.GetAddressOf()));
            assert(SUCCEEDED(hr) && result != nullptr);

            hr = result->GetResult(specConstantLibraryBlob.GetAddressOf());
            assert(SUCCEEDED(hr) && specConstantLibraryBlob != nullptr);

            std::lock_guard lock(g_compiledSpecConstantLibraryBlobMutex);
            g_compiledSpecConstantLibraryBlobs.emplace(specConstants, specConstantLibraryBlob);
        }

        if (s_dxcLinker == nullptr)
        {
            HRESULT hr = DxcCreateInstance(CLSID_DxcLinker, IID_PPV_ARGS(s_dxcLinker.GetAddressOf()));
            assert(SUCCEEDED(hr) && s_dxcLinker != nullptr);
        }

        s_dxcLinker->RegisterLibrary(specConstantsLibName, specConstantLibraryBlob.Get());

        wchar_t shaderLibName[0x100];
        swprintf_s(shaderLibName, L"Shader_%d", guestShader->shaderCacheEntry->dxilOffset);

        ComPtr<IDxcBlobEncoding> shaderLibraryBlob;
        {
            std::lock_guard lock(guestShader->mutex);
            shaderLibraryBlob = guestShader->libraryBlob;
        }

        if (shaderLibraryBlob == nullptr)
        {
            if (s_dxcUtils == nullptr)
            {
                HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(s_dxcUtils.GetAddressOf()));
                assert(SUCCEEDED(hr) && s_dxcUtils != nullptr);
            }

            HRESULT hr = s_dxcUtils->CreateBlobFromPinned(
                g_shaderCache.get() + guestShader->shaderCacheEntry->dxilOffset,
                guestShader->shaderCacheEntry->dxilSize,
                DXC_CP_ACP,
                shaderLibraryBlob.GetAddressOf());

            assert(SUCCEEDED(hr) && shaderLibraryBlob != nullptr);

            std::lock_guard lock(guestShader->mutex);
            guestShader->libraryBlob = shaderLibraryBlob;
        }

        s_dxcLinker->RegisterLibrary(shaderLibName, shaderLibraryBlob.Get());

        const wchar_t* libraryNames[] = { specConstantsLibName, shaderLibName };

        ComPtr<IDxcOperationResult> result;
        HRESULT hr = s_dxcLinker->Link(L"shaderMain", guestShader->type == ResourceType::VertexShader ? L"vs_6_0" : L"ps_6_0",
            libraryNames, std::size(libraryNames), nullptr, 0, result.GetAddressOf());

        assert(SUCCEEDED(hr) && result != nullptr);

        ComPtr<IDxcBlob> blob;
        hr = result->GetResult(blob.GetAddressOf());
        assert(SUCCEEDED(hr) && blob != nullptr);

        {
            std::lock_guard lock(guestShader->mutex);

            auto& linkedShader = guestShader->linkedShaders[specConstants];
            if (linkedShader == nullptr)
            {
                linkedShader = g_device->createShader(blob->GetBufferPointer(), blob->GetBufferSize(), "shaderMain", RenderShaderFormat::DXIL);
                guestShader->shaderBlobs.push_back(std::move(blob));
            }

            shader = linkedShader.get();

#ifdef _DEBUG
            shader->setName(fmt::format("{}:{:x}", guestShader->shaderCacheEntry->filename, guestShader->shaderCacheEntry->hash));
#endif
        }        
    }
#endif

    return shader;
}

static XXH64_hash_t GetPipelineStateHash(const PipelineState& pipelineState)
{
    XXH64_hash_t seed = 0;

    // If shaders need to be replaced, make sure they result in a different pipeline hash.
    struct {
        RenderShader* vertex;
        RenderShader* pixel;
    } replacements(GetReplacementShader(pipelineState.vertexShader), GetReplacementShader(pipelineState.pixelShader));
    if (replacements.vertex != nullptr || replacements.pixel != nullptr)
        seed = XXH3_64bits(&replacements, sizeof(replacements));

    return XXH3_64bits_withSeed(&pipelineState, sizeof(pipelineState), seed);
}

static void SanitizePipelineState(PipelineState& pipelineState)
{
    if (!pipelineState.zEnable && !pipelineState.stencilEnable)
    {
        pipelineState.depthStencilFormat = RenderFormat::UNKNOWN;
    }

    if (!pipelineState.zEnable)
    {
        pipelineState.zWriteEnable = false;
        pipelineState.zFunc = RenderComparisonFunction::LESS;
        pipelineState.slopeScaledDepthBias = 0.0f;
        pipelineState.depthBias = 0;
    }

    if (!pipelineState.stencilEnable)
    {
        pipelineState.stencilTwoSided = false;
        pipelineState.stencilFunc = RenderComparisonFunction::ALWAYS;
        pipelineState.stencilFail = RenderStencilOp::KEEP;
        pipelineState.stencilZFail = RenderStencilOp::KEEP;
        pipelineState.stencilPass = RenderStencilOp::KEEP;
        pipelineState.stencilMask = 0xFFFFFFFF;
        pipelineState.stencilWriteMask = 0xFFFFFFFF;
        pipelineState.stencilRef = 0;
    }

    if (!pipelineState.stencilTwoSided)
    {
        pipelineState.stencilFuncCCW = pipelineState.stencilFunc;
        pipelineState.stencilFailCCW = pipelineState.stencilFail;
        pipelineState.stencilZFailCCW = pipelineState.stencilZFail;
        pipelineState.stencilPassCCW = pipelineState.stencilPass;
    }

    if (pipelineState.slopeScaledDepthBias == 0.0f)
        pipelineState.slopeScaledDepthBias = 0.0f; // Remove sign.

    if (!pipelineState.colorWriteEnable)
    {
        pipelineState.alphaBlendEnable = false;
        pipelineState.renderTargetFormat = RenderFormat::UNKNOWN;
    }

    if (!pipelineState.alphaBlendEnable)
    {
        pipelineState.srcBlend = RenderBlend::ONE;
        pipelineState.destBlend = RenderBlend::ZERO;
        pipelineState.blendOp = RenderBlendOperation::ADD;
        pipelineState.srcBlendAlpha = RenderBlend::ONE;
        pipelineState.destBlendAlpha = RenderBlend::ZERO;
        pipelineState.blendOpAlpha = RenderBlendOperation::ADD;
    }

    for (size_t i = 0; i < 16; i++)
    {
        if (!pipelineState.vertexDeclaration->vertexStreams[i])
            pipelineState.vertexStrides[i] = 0;
    }

    uint32_t specConstantsMask = 0;
    if (pipelineState.vertexShader->shaderCacheEntry != nullptr)
        specConstantsMask |= pipelineState.vertexShader->shaderCacheEntry->specConstantsMask;

    if (pipelineState.pixelShader != nullptr && pipelineState.pixelShader->shaderCacheEntry != nullptr)
        specConstantsMask |= pipelineState.pixelShader->shaderCacheEntry->specConstantsMask;

    pipelineState.specConstants &= specConstantsMask;
}

static std::unique_ptr<RenderPipeline> CreateGraphicsPipeline(const PipelineState& pipelineState)
{
#ifdef ASYNC_PSO_DEBUG
    ++g_pipelinesCurrentlyCompiling;
#endif

    RenderGraphicsPipelineDesc desc;
    desc.pipelineLayout = g_pipelineLayout.get();
    desc.vertexShader = GetOrLinkShader(pipelineState.vertexShader, pipelineState.specConstants);
    if (pipelineState.enableConditionalSurvey)
        desc.pixelShader = GetOrLinkShader(g_conditionalSurveyPSShader.get(), pipelineState.specConstants);
    else if (pipelineState.pixelShader != nullptr)
        desc.pixelShader = GetOrLinkShader(pipelineState.pixelShader, pipelineState.specConstants);
    else
        desc.pixelShader = nullptr;
    desc.depthFunction = pipelineState.zFunc;
    desc.depthEnabled = pipelineState.zEnable;
    desc.depthWriteEnabled = pipelineState.zWriteEnable;
    desc.depthBias = pipelineState.depthBias;
    desc.stencilEnabled = pipelineState.stencilEnable;
    desc.stencilReadMask = pipelineState.stencilMask;
    desc.stencilWriteMask = pipelineState.stencilWriteMask;
    desc.stencilReference = pipelineState.stencilRef;
    desc.stencilFrontFace.compareFunction = pipelineState.stencilFunc;
    desc.stencilFrontFace.failOp = pipelineState.stencilFail;
    desc.stencilFrontFace.depthFailOp = pipelineState.stencilZFail;
    desc.stencilFrontFace.passOp = pipelineState.stencilPass;
    if (pipelineState.stencilTwoSided) {
        desc.stencilBackFace.compareFunction = pipelineState.stencilFuncCCW;
        desc.stencilBackFace.failOp = pipelineState.stencilFailCCW;
        desc.stencilBackFace.depthFailOp = pipelineState.stencilZFailCCW;
        desc.stencilBackFace.passOp = pipelineState.stencilPassCCW;
    } else {
        desc.stencilBackFace = desc.stencilFrontFace;
    }
    desc.slopeScaledDepthBias = pipelineState.slopeScaledDepthBias;
    desc.dynamicDepthBiasEnabled = g_capabilities.dynamicDepthBias;
    desc.depthClipEnabled = true;
    desc.primitiveTopology = pipelineState.primitiveTopology;
    if (!g_capabilities.triangleFan && desc.primitiveTopology == RenderPrimitiveTopology::TRIANGLE_FAN) {
        desc.primitiveTopology = RenderPrimitiveTopology::TRIANGLE_LIST;
    }
    desc.cullMode = pipelineState.cullMode;
    desc.frontFace = pipelineState.frontFace;
    desc.renderTargetFormat[0] = pipelineState.renderTargetFormat;
    desc.renderTargetBlend[0].blendEnabled = pipelineState.alphaBlendEnable;
    desc.renderTargetBlend[0].srcBlend = pipelineState.srcBlend;
    desc.renderTargetBlend[0].dstBlend = pipelineState.destBlend;
    desc.renderTargetBlend[0].blendOp = pipelineState.blendOp;
    desc.renderTargetBlend[0].srcBlendAlpha = pipelineState.srcBlendAlpha;
    desc.renderTargetBlend[0].dstBlendAlpha = pipelineState.destBlendAlpha;
    desc.renderTargetBlend[0].blendOpAlpha = pipelineState.blendOpAlpha;
    desc.renderTargetBlend[0].renderTargetWriteMask = pipelineState.colorWriteEnable;
    desc.renderTargetCount = pipelineState.renderTargetFormat != RenderFormat::UNKNOWN ? 1 : 0;
    desc.depthTargetFormat = pipelineState.depthStencilFormat;
    desc.multisampling.sampleCount = pipelineState.sampleCount;
    desc.alphaToCoverageEnabled = pipelineState.enableAlphaToCoverage;
    desc.inputElements = pipelineState.vertexDeclaration->inputElements.get();
    desc.inputElementsCount = pipelineState.vertexDeclaration->inputElementCount;
    
    RenderSpecConstant specConstant{};
    specConstant.value = pipelineState.specConstants;
    
    if (pipelineState.specConstants != 0)
    {
        desc.specConstants = &specConstant;
        desc.specConstantsCount = 1;
    }
    
    RenderInputSlot inputSlots[16]{};
    uint32_t inputSlotIndices[16]{};
    uint32_t inputSlotCount = 0;
    
    for (size_t i = 0; i < pipelineState.vertexDeclaration->inputElementCount; i++)
    {
        auto& inputElement = pipelineState.vertexDeclaration->inputElements[i];
        auto& inputSlotIndex = inputSlotIndices[inputElement.slotIndex];
    
        if (inputSlotIndex == NULL)
            inputSlotIndex = ++inputSlotCount;
    
        auto& inputSlot = inputSlots[inputSlotIndex - 1];
        inputSlot.index = inputElement.slotIndex;
        inputSlot.stride = pipelineState.vertexStrides[inputElement.slotIndex];
        inputSlot.classification = RenderInputSlotClassification::PER_VERTEX_DATA;
    }
    
    desc.inputSlots = inputSlots;
    desc.inputSlotsCount = inputSlotCount;
    
    auto pipeline = g_device->createGraphicsPipeline(desc);

#ifdef ASYNC_PSO_DEBUG
    --g_pipelinesCurrentlyCompiling;
#endif

    return pipeline;
}

static RenderPipeline* CreateGraphicsPipelineInRenderThread(PipelineState pipelineState)
{
    SanitizePipelineState(pipelineState);

    XXH64_hash_t hash = GetPipelineStateHash(pipelineState);
    auto& pipeline = g_pipelines[hash];
    if (pipeline == nullptr)
    {
        pipeline = CreateGraphicsPipeline(pipelineState);

#ifdef ASYNC_PSO_DEBUG
        if (App::s_isLoading)
            ++g_pipelinesCreatedAsynchronously;
        else
            ++g_pipelinesCreatedInRenderThread;

        pipeline->setName(fmt::format("{} {} {} {:X}", App::s_isLoading ? "ASYNC" : "",
            pipelineState.vertexShader->name, pipelineState.pixelShader != nullptr ? pipelineState.pixelShader->name : "<none>", hash));
        
        if (!App::s_isLoading)
        {
            std::lock_guard lock(g_debugMutex);
            g_pipelineDebugText = fmt::format(
                "{:X}\n"
                "    vertexShader: {}\n"
                "    pixelShader: {}\n"
                "    vertexDeclaration: {:X}\n"
                "    zEnable: {}\n"
                "    zWriteEnable: {}\n"
                "    stencilEnable: {}\n"
                "    stencilTwoSided: {}\n"
                "    srcBlend: {}\n"
                "    destBlend: {}\n"
                "    cullMode: {}\n"
                "    frontFace: {}\n"
                "    zFunc: {}\n"
                "    stencilFunc: {}\n"
                "    stencilFail: {}\n"
                "    stencilZFail: {}\n"
                "    stencilPass: {}\n"
                "    stencilFuncCCW: {}\n"
                "    stencilFailCCW: {}\n"
                "    stencilZFailCCW: {}\n"
                "    stencilPassCCW: {}\n"
                "    stencilMask: {}\n"
                "    stencilWriteMask: {}\n"
                "    stencilRef: {}\n"
                "    alphaBlendEnable: {}\n"
                "    blendOp: {}\n"
                "    slopeScaledDepthBias: {}\n"
                "    depthBias: {}\n"
                "    srcBlendAlpha: {}\n"
                "    destBlendAlpha: {}\n"
                "    blendOpAlpha: {}\n"
                "    colorWriteEnable: {:X}\n"
                "    primitiveTopology: {}\n"
                "    vertexStrides[0]: {}\n"
                "    vertexStrides[1]: {}\n"
                "    vertexStrides[2]: {}\n"
                "    vertexStrides[3]: {}\n"
                "    renderTargetFormat: {}\n"
                "    depthStencilFormat: {}\n"
                "    sampleCount: {}\n"
                "    enableAlphaToCoverage: {}\n"
                "    enableConditionalSurvey: {}\n"
                "    specConstants: {:X}\n\n",
                hash,
                pipelineState.vertexShader->name,
                pipelineState.pixelShader != nullptr ? pipelineState.pixelShader->name : "<none>",
                reinterpret_cast<size_t>(pipelineState.vertexDeclaration),
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
                magic_enum::enum_name(pipelineState.renderTargetFormat),
                magic_enum::enum_name(pipelineState.depthStencilFormat),
                pipelineState.sampleCount,
                pipelineState.enableAlphaToCoverage,
                pipelineState.enableConditionalSurvey,
                pipelineState.specConstants)
                + g_pipelineDebugText;
        }
#endif

#ifdef PSO_CACHING
        std::lock_guard lock(g_pipelineCacheMutex);
        g_pipelineStatesToCache.emplace(hash, pipelineState);
#endif
    }
    
    return pipeline.get();
}

static void ProcRenderCommand(const RenderCommands::SetBooleans& args)
{
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.booleans, args.booleans);
}

static void ProcRenderCommand(const RenderCommands::SetSamplerState& args)
{
    const auto addressU = ConvertTextureAddressMode((args.data0 >> 10) & 0x7);
    const auto addressV = ConvertTextureAddressMode((args.data0 >> 13) & 0x7);
    const auto addressW = ConvertTextureAddressMode((args.data0 >> 16) & 0x7);
    auto magFilter = ConvertTextureFilter((args.data3 >> 19) & 0x3);
    auto minFilter = ConvertTextureFilter((args.data3 >> 21) & 0x3);
    auto mipFilter = ConvertTextureFilter((args.data3 >> 23) & 0x3);
    const auto borderColor = ConvertBorderColor(args.data5 & 0x3);

    bool anisotropyEnabled = Config::AnisotropicFiltering > 0 && mipFilter == RenderFilter::LINEAR;
    if (anisotropyEnabled)
    {
        magFilter = RenderFilter::LINEAR;
        minFilter = RenderFilter::LINEAR;
    }

    auto& samplerDesc = g_samplerDescs[args.index];

    bool dirty = false;

    SetDirtyValue(dirty, samplerDesc.addressU, addressU);
    SetDirtyValue(dirty, samplerDesc.addressV, addressV);
    SetDirtyValue(dirty, samplerDesc.addressW, addressW);
    SetDirtyValue(dirty, samplerDesc.minFilter, minFilter);
    SetDirtyValue(dirty, samplerDesc.magFilter, magFilter);
    SetDirtyValue(dirty, samplerDesc.mipmapMode, RenderMipmapMode(mipFilter));
    SetDirtyValue(dirty, samplerDesc.maxAnisotropy, anisotropyEnabled ? Config::AnisotropicFiltering : 16u);
    SetDirtyValue(dirty, samplerDesc.anisotropyEnabled, anisotropyEnabled);
    SetDirtyValue(dirty, samplerDesc.borderColor, borderColor);

    if (dirty)
    {
        auto& [descriptorIndex, sampler] = g_samplerStates[XXH3_64bits(&samplerDesc, sizeof(RenderSamplerDesc))];
        if (descriptorIndex == NULL)
        {
            descriptorIndex = g_samplerStates.size();
            sampler = g_device->createSampler(samplerDesc);

            g_samplerDescriptorSet->setSampler(descriptorIndex - 1, sampler.get());
        }

        SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.samplerIndices[args.index], descriptorIndex - 1);
    }
}

static void ProcRenderCommand(const RenderCommands::SetVertexShaderConstants& args)
{
    assert((args.index * sizeof(uint32_t) + args.size) <= sizeof(g_vertexShaderConstants));

    memcpy(&g_vertexShaderConstants[args.index], args.memory, args.size);
    g_dirtyStates.vertexShaderConstants = true;
}

static void ProcRenderCommand(const RenderCommands::SetPixelShaderConstants& args)
{
    assert((args.index * sizeof(uint32_t) + args.size) <= sizeof(g_pixelShaderConstants));

    memcpy(&g_pixelShaderConstants[args.index], args.memory, args.size);
    g_dirtyStates.pixelShaderConstants = true;
}

static void ProcRenderCommand(const RenderCommands::AddPipeline& args)
{
    auto& pipeline = g_pipelines[args.hash];

    if (pipeline == nullptr)
    {
        pipeline = std::unique_ptr<RenderPipeline>(args.pipeline);
#ifdef ASYNC_PSO_DEBUG
        ++g_pipelinesCreatedAsynchronously;
#endif
    }
    else
    {
#ifdef ASYNC_PSO_DEBUG
        ++g_pipelinesDropped;
#endif
        delete args.pipeline;
    }
}

static constexpr int32_t COMMON_DEPTH_BIAS_VALUE = int32_t((1 << 24) * 0.002f);
static constexpr float COMMON_SLOPE_SCALED_DEPTH_BIAS_VALUE = 1.0f;

static void FlushRenderStateForRenderThread()
{
    auto renderTarget = g_pipelineState.colorWriteEnable ? g_renderTarget : nullptr;
    auto depthStencil = g_pipelineState.zEnable || g_pipelineState.stencilEnable ? g_depthStencil : nullptr;

    bool foundAny = PopulateBarriersForStretchRect(renderTarget, depthStencil);

    auto aliasesBoundStorage = [&](GuestSurface* surface)
    {
        return surface != renderTarget && surface != depthStencil &&
            (SharesStorage(surface, renderTarget) || SharesStorage(surface, depthStencil));
    };

    for (const auto surface : g_pendingSurfaceCopies)
    {
        if (aliasesBoundStorage(surface))
        {
            bool isDepthStencil = RenderFormatIsDepth(surface->format);
            foundAny |= PopulateBarriersForStretchRect(isDepthStencil ? nullptr : surface, isDepthStencil ? surface : nullptr);
        }
    }

    for (const auto surface : g_pendingResolves)
    {
        bool isDepthStencil = RenderFormatIsDepth(surface->format);
        foundAny |= PopulateBarriersForStretchRect(isDepthStencil ? nullptr : surface, isDepthStencil ? surface : nullptr);
    }

    if (foundAny)
    {
        FlushBarriers();
        ExecutePendingStretchRectCommands(renderTarget, depthStencil);

        for (const auto surface : g_pendingSurfaceCopies)
        {
            if (aliasesBoundStorage(surface))
            {
                bool isDepthStencil = RenderFormatIsDepth(surface->format);
                ExecutePendingStretchRectCommands(isDepthStencil ? nullptr : surface, isDepthStencil ? surface : nullptr);
            }
        }

        for (const auto surface : g_pendingResolves)
        {
            bool isDepthStencil = RenderFormatIsDepth(surface->format);
            ExecutePendingStretchRectCommands(isDepthStencil ? nullptr : surface, isDepthStencil ? surface : nullptr);
        }
    }

    if (!g_pendingResolves.empty())
        g_pendingResolves.clear();

    AddBarrier(renderTarget, RenderTextureLayout::COLOR_WRITE);
    AddBarrier(depthStencil, RenderTextureLayout::DEPTH_WRITE);

    FlushBarriers();

    SetFramebuffer(renderTarget, depthStencil, false);
    ApplyDeferredVariantClears(renderTarget, depthStencil);
    FlushViewport();

    auto& commandList = g_commandLists[g_frame];

    // D3D12 resets depth bias values to the pipeline values, even if they are dynamic.
    // We can reduce unnecessary calls by making common depth bias values part of the pipeline.
    if (g_capabilities.dynamicDepthBias && g_backend == Backend::D3D12)
    {
        bool useDepthBias = (g_depthBias != 0) || (g_slopeScaledDepthBias != 0.0f);

        int32_t depthBias = useDepthBias ? COMMON_DEPTH_BIAS_VALUE : 0;
        float slopeScaledDepthBias = useDepthBias ? COMMON_SLOPE_SCALED_DEPTH_BIAS_VALUE : 0.0f;

        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.depthBias, depthBias);
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.slopeScaledDepthBias, slopeScaledDepthBias);
    }

    if (g_dirtyStates.pipelineState)
    {
        commandList->setPipeline(CreateGraphicsPipelineInRenderThread(g_pipelineState));

        // D3D12 resets the depth bias values. Check if they need to be set again.
        if (g_capabilities.dynamicDepthBias && g_backend == Backend::D3D12)
            g_dirtyStates.depthBias = (g_depthBias != g_pipelineState.depthBias) || (g_slopeScaledDepthBias != g_pipelineState.slopeScaledDepthBias);
    }

    if (g_dirtyStates.depthBias && g_capabilities.dynamicDepthBias)
        commandList->setDepthBias(g_depthBias, 0.0f, g_slopeScaledDepthBias);

    if (g_dirtyStates.vertexShaderConstants)
    {
        auto vertexShaderConstants = g_uploadAllocators[g_frame].allocate<true>(g_vertexShaderConstants, sizeof(g_vertexShaderConstants), 0x100);
        SetRootDescriptor(vertexShaderConstants, 0);
    }

    if (g_dirtyStates.pixelShaderConstants)
    {
        auto pixelShaderConstants = g_uploadAllocators[g_frame].allocate<true>(g_pixelShaderConstants, sizeof(g_pixelShaderConstants), 0x100);
        SetRootDescriptor(pixelShaderConstants, 1);
    }

    if (g_dirtyStates.sharedConstants)
    {
        auto sharedConstants = g_uploadAllocators[g_frame].allocate<false>(&g_sharedConstants, sizeof(g_sharedConstants), 0x100);
        SetRootDescriptor(sharedConstants, 2);
    }

    if (g_dirtyStates.vertexStreamFirst <= g_dirtyStates.vertexStreamLast)
    {
        commandList->setVertexBuffers(
            g_dirtyStates.vertexStreamFirst,
            g_vertexBufferViews + g_dirtyStates.vertexStreamFirst,
            g_dirtyStates.vertexStreamLast - g_dirtyStates.vertexStreamFirst + 1,
            g_inputSlots + g_dirtyStates.vertexStreamFirst);
    }

    if (g_dirtyStates.indices && (g_backend == Backend::D3D12 || g_indexBufferView.buffer.ref != nullptr))
        commandList->setIndexBuffer(&g_indexBufferView);

    g_dirtyStates = DirtyStates(false);
}

static void SetPrimitiveType(uint32_t primitiveType)
{
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.primitiveTopology, ConvertPrimitiveType(primitiveType));
}

static void ProcRenderCommand(const RenderCommands::DrawPrimitive& args)
{
    SetPrimitiveType(args.primitiveType);

    FlushRenderStateForRenderThread();

    auto& commandList = g_commandLists[g_frame];
    commandList->drawInstanced(args.primitiveCount, 1, args.startVertex, 0);
}

static void ProcRenderCommand(const RenderCommands::DrawIndexedPrimitive& args)
{
    SetPrimitiveType(args.primitiveType);
    FlushRenderStateForRenderThread();

    g_commandLists[g_frame]->drawIndexedInstanced(args.primCount, 1, args.startIndex, args.baseVertexIndex, 0);
}

static void ProcRenderCommand(const RenderCommands::DrawPrimitiveUP& args)
{
    SetPrimitiveType(args.primitiveType);
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.vertexStrides[0], uint8_t(args.vertexStreamZeroStride));

    auto allocation = g_uploadAllocators[g_frame].allocate<true>(reinterpret_cast<const uint32_t*>(args.vertexStreamZeroData), args.vertexStreamZeroSize, 0x4);

    auto& vertexBufferView = g_vertexBufferViews[0];
    vertexBufferView.size = args.primitiveCount * args.vertexStreamZeroStride;
    vertexBufferView.buffer = allocation.buffer->at(allocation.offset);
    g_inputSlots[0].stride = args.vertexStreamZeroStride;
    g_dirtyStates.vertexStreamFirst = 0;

    uint32_t indexCount = 0;

    if (args.primitiveType == D3DPT_QUADLIST)
        indexCount = g_quadIndexData.prepare(args.primitiveCount);
    else if (!g_capabilities.triangleFan && args.primitiveType == D3DPT_TRIANGLEFAN)
        indexCount = g_triangleFanIndexData.prepare(args.primitiveCount);

    FlushRenderStateForRenderThread();

    if (indexCount != 0)
        g_commandLists[g_frame]->drawIndexedInstanced(indexCount, 1, 0, 0, 0);
    else
        g_commandLists[g_frame]->drawInstanced(args.primitiveCount, 1, 0, 0);
}

static GuestVertexDeclaration* CreateVertexDeclarationWithoutAddRef(GuestVertexElement* vertexElements) 
{
    size_t vertexElementCount = 0;
    auto vertexElement = vertexElements;

    while (vertexElement->stream != 0xFF && vertexElement->type != D3DDECLTYPE_UNUSED)
    {
        vertexElement->padding = 0;
        ++vertexElement;
        ++vertexElementCount;
    }

    vertexElement->padding = 0; // Clear the padding in D3DDECL_END() 

    std::lock_guard lock(g_vertexDeclarationMutex);

    XXH64_hash_t hash = XXH3_64bits(vertexElements, vertexElementCount * sizeof(GuestVertexElement));
    auto& vertexDeclaration = g_vertexDeclarations[hash];

    if (vertexDeclaration == nullptr)
    {
        vertexDeclaration = g_userHeap.AllocPhysical<GuestVertexDeclaration>(ResourceType::VertexDeclaration);
        vertexDeclaration->hash = hash;

        static std::vector<RenderInputElement> inputElements;
        inputElements.clear();

        struct Location
        {
            uint32_t usage;
            uint32_t usageIndex;
            uint32_t location;
        };

        // Should match the locations defined in XenosRecomp.
        constexpr Location locations[] =
        {
            { D3DDECLUSAGE_POSITION, 0, 0 },
            { D3DDECLUSAGE_POSITION, 1, 1 },
            { D3DDECLUSAGE_POSITION, 2, 2 },
            { D3DDECLUSAGE_POSITION, 3, 3 },
            { D3DDECLUSAGE_NORMAL, 0, 4 },
            { D3DDECLUSAGE_NORMAL, 1, 5 },
            { D3DDECLUSAGE_NORMAL, 2, 6 },
            { D3DDECLUSAGE_NORMAL, 3, 7 },
            { D3DDECLUSAGE_TANGENT, 0, 8 },
            { D3DDECLUSAGE_TANGENT, 1, 9 },
            { D3DDECLUSAGE_TANGENT, 2, 10 },
            { D3DDECLUSAGE_TANGENT, 3, 11 },
            { D3DDECLUSAGE_BINORMAL, 0, 12 },
            { D3DDECLUSAGE_TEXCOORD, 0, 13 },
            { D3DDECLUSAGE_TEXCOORD, 1, 14 },
            { D3DDECLUSAGE_TEXCOORD, 2, 15 },
            { D3DDECLUSAGE_TEXCOORD, 3, 16 },
            { D3DDECLUSAGE_COLOR, 0, 17 },
            { D3DDECLUSAGE_BLENDINDICES, 0, 18 },
            { D3DDECLUSAGE_BLENDWEIGHT, 0, 19 },
        };

        vertexElement = vertexElements;
        while (vertexElement->stream != 0xFF && vertexElement->type != D3DDECLTYPE_UNUSED)
        {
            uint32_t resolvedLocation = ~0;
            for (auto& location : locations)
            {
                if (location.usage == vertexElement->usage && location.usageIndex == vertexElement->usageIndex)
                {
                    resolvedLocation = location.location;
                    break;
                }
            }

            if (resolvedLocation == ~0)
            {
                // Bound but not used by any guest shaders.
                ++vertexElement;
                continue;
            }

            auto& inputElement = inputElements.emplace_back();
            inputElement.semanticName = ConvertDeclUsage(vertexElement->usage);
            inputElement.semanticIndex = vertexElement->usageIndex;
            inputElement.location = resolvedLocation;
            inputElement.format = ConvertDeclType(vertexElement->type);
            inputElement.slotIndex = vertexElement->stream;
            inputElement.alignedByteOffset = vertexElement->offset;

            switch (vertexElement->usage)
            {
            case D3DDECLUSAGE_NORMAL:
                switch (vertexElement->type)
                {
                case D3DDECLTYPE_SHORT2:
                case D3DDECLTYPE_SHORT4:
                case D3DDECLTYPE_SHORT2N:
                case D3DDECLTYPE_SHORT4N:
                case D3DDECLTYPE_USHORT2N:
                case D3DDECLTYPE_USHORT4N:
                case D3DDECLTYPE_FLOAT16_2:
                case D3DDECLTYPE_FLOAT16_4:
                    vertexDeclaration->swappedNormals |= 1 << vertexElement->usageIndex;
                    break;
                }

                break;
            case D3DDECLUSAGE_BINORMAL:
                switch (vertexElement->type)
                {
                case D3DDECLTYPE_SHORT2:
                case D3DDECLTYPE_SHORT4:
                case D3DDECLTYPE_SHORT2N:
                case D3DDECLTYPE_SHORT4N:
                case D3DDECLTYPE_USHORT2N:
                case D3DDECLTYPE_USHORT4N:
                case D3DDECLTYPE_FLOAT16_2:
                case D3DDECLTYPE_FLOAT16_4:
                    vertexDeclaration->swappedBinormals |= 1 << vertexElement->usageIndex;
                    break;
                }

                break;
            case D3DDECLUSAGE_TANGENT:
                switch (vertexElement->type)
                {
                case D3DDECLTYPE_SHORT2:
                case D3DDECLTYPE_SHORT4:
                case D3DDECLTYPE_SHORT2N:
                case D3DDECLTYPE_SHORT4N:
                case D3DDECLTYPE_USHORT2N:
                case D3DDECLTYPE_USHORT4N:
                case D3DDECLTYPE_FLOAT16_2:
                case D3DDECLTYPE_FLOAT16_4:
                    vertexDeclaration->swappedTangents |= 1 << vertexElement->usageIndex;
                    break;
                }

                break;
            case D3DDECLUSAGE_BLENDWEIGHT:
                switch (vertexElement->type)
                {
                case D3DDECLTYPE_SHORT2:
                case D3DDECLTYPE_SHORT4:
                case D3DDECLTYPE_SHORT2N:
                case D3DDECLTYPE_SHORT4N:
                case D3DDECLTYPE_USHORT2N:
                case D3DDECLTYPE_USHORT4N:
                case D3DDECLTYPE_FLOAT16_2:
                case D3DDECLTYPE_FLOAT16_4:
                    vertexDeclaration->swappedBlendWeights |= 1 << vertexElement->usageIndex;
                    break;
                }

                break;

            case D3DDECLUSAGE_TEXCOORD:
                switch (vertexElement->type)
                {
                case D3DDECLTYPE_SHORT2:
                case D3DDECLTYPE_SHORT4:
                case D3DDECLTYPE_SHORT2N:
                case D3DDECLTYPE_SHORT4N:
                case D3DDECLTYPE_USHORT2N:
                case D3DDECLTYPE_USHORT4N:
                case D3DDECLTYPE_FLOAT16_2:
                case D3DDECLTYPE_FLOAT16_4:
                    vertexDeclaration->swappedTexcoords |= 1 << vertexElement->usageIndex;
                    break;
                }

                break;
            }

            vertexDeclaration->vertexStreams[vertexElement->stream] = true;

            ++vertexElement;
        }

        auto addInputElement = [&](uint32_t usage, uint32_t usageIndex)
            {
                uint32_t location = ~0;

                for (auto& alsoLocation : locations)
                {
                    if (alsoLocation.usage == usage && alsoLocation.usageIndex == usageIndex)
                    {
                        location = alsoLocation.location;
                        break;
                    }
                }

                assert(location != ~0);

                for (auto& inputElement : inputElements)
                {
                    if (inputElement.location == location)
                        return;
                }

                auto format = RenderFormat::R32_FLOAT;
                switch (usage)
                {
                case D3DDECLUSAGE_NORMAL:
                case D3DDECLUSAGE_TANGENT:
                case D3DDECLUSAGE_BINORMAL:
                case D3DDECLUSAGE_BLENDINDICES:
                    format = RenderFormat::R32G32B32_FLOAT;
                    break;
                }

                inputElements.emplace_back(ConvertDeclUsage(usage), usageIndex, location, format, 15, 0);
            };

        // Assign any unbound usages to null buffer slot.
        for (auto& location : locations)
        {
            addInputElement(location.usage, location.usageIndex);
        }

        vertexDeclaration->inputElements = std::make_unique<RenderInputElement[]>(inputElements.size());
        std::copy(inputElements.begin(), inputElements.end(), vertexDeclaration->inputElements.get());

        vertexDeclaration->vertexElements = std::make_unique<GuestVertexElement[]>(vertexElementCount + 1);
        std::copy(vertexElements, vertexElements + vertexElementCount + 1, vertexDeclaration->vertexElements.get());

        vertexDeclaration->inputElementCount = uint32_t(inputElements.size());
        vertexDeclaration->vertexElementCount = vertexElementCount + 1;
    }

    vertexDeclaration->AddRef();
    return vertexDeclaration;
}

static GuestVertexDeclaration* CreateVertexDeclaration(GuestVertexElement* vertexElements)
{
    auto vertexDeclaration = CreateVertexDeclarationWithoutAddRef(vertexElements);
    vertexDeclaration->AddRef();
    return vertexDeclaration;
}

static void ProcRenderCommand(const RenderCommands::SetVertexDeclaration& args)
{
    if (args.vertexDeclaration != nullptr)
    {
        SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.swappedTexcoords, args.vertexDeclaration->swappedTexcoords);
        SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.swappedNormals, args.vertexDeclaration->swappedNormals);
        SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.swappedBinormals, args.vertexDeclaration->swappedBinormals);
        SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.swappedTangents, args.vertexDeclaration->swappedTangents);
        SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.swappedBlendWeights, args.vertexDeclaration->swappedBlendWeights);

        uint32_t specConstants = g_pipelineState.specConstants;
        if (args.vertexDeclaration->hasR11G11B10Normal)
            specConstants |= SPEC_CONSTANT_R11G11B10_NORMAL;
        else
            specConstants &= ~SPEC_CONSTANT_R11G11B10_NORMAL;

        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.specConstants, specConstants);
    }
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.vertexDeclaration, args.vertexDeclaration);
}

static ShaderCacheEntry* FindShaderCacheEntry(XXH64_hash_t hash)
{
    auto end = g_shaderCacheEntries + g_shaderCacheEntryCount;
    auto findResult = std::lower_bound(g_shaderCacheEntries, end, hash, [](ShaderCacheEntry& lhs, XXH64_hash_t rhs)
        {
            return lhs.hash < rhs;
        });

    return findResult != end && findResult->hash == hash ? findResult : nullptr;
}

static GuestShader* GetOrCreateShader(XXH64_hash_t hash, ResourceType resourceType) {
    auto findResult = FindShaderCacheEntry(hash);
    GuestShader* shader = nullptr;

    if (findResult == nullptr)
    {
        LOGF_WARNING("Shader not found by hash: {:x}", hash);
        LOG_WARNING("Perhaps the path to the required shader will be printed before this error");
        __builtin_trap();
    }
    if (findResult != nullptr)
    {
        if (findResult->guestShader == nullptr)
        {
            shader = g_userHeap.AllocPhysical<GuestShader>(resourceType);
            shader->shaderCacheEntry = findResult;
#ifdef ASYNC_PSO_DEBUG
            shader->name = findResult->filename;
#endif

            findResult->guestShader = shader;
        }
        else
        {
            shader = findResult->guestShader;
        }
    }

    if (shader == nullptr)
        shader = g_userHeap.AllocPhysical<GuestShader>(resourceType);
    else
        shader->AddRef();

    return shader;
}

static GuestShader* CreateShader(const be<uint32_t>* function, ResourceType resourceType)
{
    XXH64_hash_t hash = XXH3_64bits(function, function[1] + function[2]);
    return GetOrCreateShader(hash, resourceType);
}

static GuestShader* CreateVertexShader(const be<uint32_t>* function) 
{
    return CreateShader(function, ResourceType::VertexShader);
}

static void ProcRenderCommand(const RenderCommands::SetVertexShader& args)
{
    GuestShader* shader = args.shader;
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.vertexShader, shader);
}

static void ProcRenderCommand(const RenderCommands::SetStreamSource& args)
{
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.vertexStrides[args.index], uint8_t(args.buffer != nullptr ? args.stride : 0));

    bool dirty = false;

    SetDirtyValue(dirty, g_vertexBufferViews[args.index].buffer, args.buffer != nullptr ? args.buffer->buffer->at(args.offset) : RenderBufferReference{});
    SetDirtyValue(dirty, g_vertexBufferViews[args.index].size, args.buffer != nullptr ? (args.buffer->dataSize - args.offset) : 0u);
    SetDirtyValue(dirty, g_inputSlots[args.index].stride, args.buffer != nullptr ? args.stride : 0u);

    if (dirty)
    {
        g_dirtyStates.vertexStreamFirst = std::min<uint8_t>(g_dirtyStates.vertexStreamFirst, args.index);
        g_dirtyStates.vertexStreamLast = std::max<uint8_t>(g_dirtyStates.vertexStreamLast, args.index);
    }
}

static void ProcRenderCommand(const RenderCommands::SetIndices& args)
{
    SetDirtyValue(g_dirtyStates.indices, g_indexBufferView.buffer, args.buffer != nullptr ? args.buffer->buffer->at(0) : RenderBufferReference{});
    SetDirtyValue(g_dirtyStates.indices, g_indexBufferView.format, args.buffer != nullptr ? args.buffer->format : RenderFormat::R16_UINT);
    SetDirtyValue(g_dirtyStates.indices, g_indexBufferView.size, args.buffer != nullptr ? args.buffer->dataSize : 0u);
}

static GuestShader* CreatePixelShader(const be<uint32_t>* function)
{
    return CreateShader(function, ResourceType::PixelShader);
}

static void ProcRenderCommand(const RenderCommands::SetPixelShader& args)
{
    GuestShader* shader = args.shader;
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.pixelShader, shader);
}

static void ProcRenderCommand(const RenderCommands::SetConditionalSurvey& args)
{
    if (args.enabled)
    {
        // Clear previous survey result first.
        auto uploadBuffer = g_device->createBuffer(RenderBufferDesc::UploadBuffer(sizeof(uint32_t)));
        memset(uploadBuffer->map(), 0, sizeof(uint32_t));
        uploadBuffer->unmap();

        auto& commandList = g_commandLists[g_frame];
        commandList->barriers(RenderBarrierStage::COPY, RenderBufferBarrier(g_conditionalSurveyBuffer.get(), RenderBufferAccess::WRITE));
        commandList->copyBufferRegion(g_conditionalSurveyBuffer->at(args.index * sizeof(uint32_t)), uploadBuffer->at(0), sizeof(uint32_t));
        commandList->barriers(RenderBarrierStage::GRAPHICS, RenderBufferBarrier(g_conditionalSurveyBuffer.get(), RenderBufferAccess::READ | RenderBufferAccess::WRITE));

        g_tempBuffers[g_frame].emplace_back(std::move(uploadBuffer));
    }

    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.enableConditionalSurvey, args.enabled);
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.conditionalSurveyIndex, args.index);
}

static void ProcRenderCommand(const RenderCommands::SetConditionalRendering& args)
{
    uint32_t specConstants = g_pipelineState.specConstants;
    if (args.enabled)
        specConstants |= SPEC_CONSTANT_CONDITIONAL_RENDERING;
    else
        specConstants &= ~SPEC_CONSTANT_CONDITIONAL_RENDERING;

    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.specConstants, specConstants);
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.conditionalRenderingIndex, args.index);
}

static void ProcRenderCommand(const RenderCommands::SetClipPlane& args)
{
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.clipPlane[0], args.plane[0]);
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.clipPlane[1], args.plane[1]);
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.clipPlane[2], args.plane[2]);
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.clipPlane[3], args.plane[3]);
}

static std::thread g_renderThread([]
    {
        os::thread::SetCurrentThreadName("Render Thread");

#ifdef _WIN32
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
#endif

        RenderCommand commands[32];

        while (true)
        {
            size_t count = g_renderQueue.wait_dequeue_bulk(commands, std::size(commands));

            for (size_t i = 0; i < count; i++)
                std::visit([](const auto& cmd) { ProcRenderCommand(cmd); }, commands[i]);
        }
    });

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

                    {
                        std::lock_guard lock(g_vertexDeclarationMutex);
                        pipelineState.vertexDeclaration = g_vertexDeclarations[reinterpret_cast<XXH64_hash_t>(pipelineState.vertexDeclaration)];
                    }

                    // Zero out depth bias for Vulkan, we only store common values for D3D12.
                    if (g_capabilities.dynamicDepthBias && g_backend != Backend::D3D12)
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

void VideoConfigValueChangedCallback(IConfigDef* config)
{
    // Config options that require internal resolution resize
    Video::s_needsResize |=
        config == &Config::AspectRatio ||
        config == &Config::ResolutionScale ||
        config == &Config::AntiAliasing ||
        config == &Config::ShadowResolution ||
        config == &Config::ReflectionResolution;

    if (Video::s_needsResize)
        Video::ComputeViewportDimensions();

    // Config options that require pipeline recompilation
    bool shouldRecompile =
        config == &Config::AntiAliasing ||
        config == &Config::TransparencyAntiAliasing ||
        config == &Config::RadialBlur;

    if (shouldRecompile)
        EnqueuePipelineTask(PipelineTaskType::RecompilePipelines);
}

GUEST_FUNCTION_HOOK(sub_8253EC98, CreateDevice);

GUEST_FUNCTION_HOOK(sub_825586B0, Video::Present);
GUEST_FUNCTION_HOOK(sub_82543B58, GetBackBuffer);
GUEST_FUNCTION_HOOK(sub_82543BA0, GetDepthStencil);

GUEST_FUNCTION_HOOK(sub_82547118, CreateVertexDeclaration);
GUEST_FUNCTION_HOOK(sub_82548700, CreateVertexShader);
GUEST_FUNCTION_HOOK(sub_82548608, CreatePixelShader);

GUEST_FUNCTION_STUB(sub_8253EB38);
GUEST_FUNCTION_STUB(sub_8253EB78);
GUEST_FUNCTION_STUB(sub_82543BE0); // SetGammaRamp
GUEST_FUNCTION_STUB(sub_82543C68); // SetGammaRamp
GUEST_FUNCTION_STUB(sub_82547278); // Set shader allocation
GUEST_FUNCTION_STUB(sub_8272FAD0);
GUEST_FUNCTION_STUB(sub_82558E00);
GUEST_FUNCTION_STUB(sub_82559928);
GUEST_FUNCTION_STUB(sub_82559C18);
GUEST_FUNCTION_STUB(sub_82700C18); // D3DXFilterTexture
GUEST_FUNCTION_STUB(sub_8253EAE0);
GUEST_FUNCTION_STUB(sub_8254D598); // BeginConditional
GUEST_FUNCTION_STUB(sub_8254D7B0); // BeginConditional
GUEST_FUNCTION_STUB(sub_8254D9D0); // BeginConditional
GUEST_FUNCTION_STUB(sub_8254DB90); // BeginConditional
GUEST_FUNCTION_STUB(sub_8254DD40); // SetScreenExtentQueryMode
