#include "video.h"

#include "imgui/imgui_common.h"
#include "imgui/imgui_snapshot.h"
#include "imgui/imgui_font_builder.h"

#include <app.h>
#include <bc_diff.h>
#include <cpu/guest_thread.h>
#include <cstdint>
#include <cstdio>
#include <decompressor.h>
#include <kernel/function.h>
#include <kernel/heap.h>
#include <hid/hid.h>
#include <kernel/memory.h>
#include <kernel/xdbf.h>
#include <plume_render_interface.h>
#include <res/bc_diff/button_bc_diff.bin.h>
#include <res/font/im_font_atlas.dds.h>
#include <shader/shader_cache.h>
#include <Marathon.h>
#include <ui/achievement_menu.h>
#include <ui/achievement_overlay.h>
#include <ui/button_guide.h>
#include <ui/fader.h>
#include <ui/imgui_utils.h>
#include <ui/installer_wizard.h>
#include <ui/message_window.h>
#include <ui/options_menu.h>
#include <ui/game_window.h>
#include <ui/black_bar.h>
#include <patches/aspect_ratio_patches.h>
#include <user/config.h>
#include <sdl_listener.h>
#include <xxHashMap.h>
#include <os/process.h>

#if defined(ASYNC_PSO_DEBUG) || defined(PSO_CACHING)
#include <magic_enum/magic_enum.hpp>
#endif

#include "../../tools/XenosRecomp/XenosRecomp/shader_common.h"

#ifdef MARATHON_RECOMP_D3D12
#include "shader/hlsl/blend_color_alpha_ps.hlsl.dxil.h"
#include "shader/hlsl/copy_vs.hlsl.dxil.h"
#include "shader/hlsl/copy_color_ps.hlsl.dxil.h"
#include "shader/hlsl/copy_depth_ps.hlsl.dxil.h"
#include "shader/hlsl/csd_filter_ps.hlsl.dxil.h"
#include "shader/hlsl/csd_no_tex_vs.hlsl.dxil.h"
#include "shader/hlsl/csd_vs.hlsl.dxil.h"
#include "shader/hlsl/enhanced_motion_blur_ps.hlsl.dxil.h"
#include "shader/hlsl/gamma_correction_ps.hlsl.dxil.h"
#include "shader/hlsl/gaussian_blur_3x3.hlsl.dxil.h"
#include "shader/hlsl/gaussian_blur_5x5.hlsl.dxil.h"
#include "shader/hlsl/gaussian_blur_7x7.hlsl.dxil.h"
#include "shader/hlsl/gaussian_blur_9x9.hlsl.dxil.h"
#include "shader/hlsl/imgui_ps.hlsl.dxil.h"
#include "shader/hlsl/imgui_vs.hlsl.dxil.h"
#include "shader/hlsl/movie_ps.hlsl.dxil.h"
#include "shader/hlsl/movie_vs.hlsl.dxil.h"
#include "shader/hlsl/resolve_msaa_color_2x.hlsl.dxil.h"
#include "shader/hlsl/resolve_msaa_color_4x.hlsl.dxil.h"
#include "shader/hlsl/resolve_msaa_color_8x.hlsl.dxil.h"
#include "shader/hlsl/resolve_msaa_depth_2x.hlsl.dxil.h"
#include "shader/hlsl/resolve_msaa_depth_4x.hlsl.dxil.h"
#include "shader/hlsl/resolve_msaa_depth_8x.hlsl.dxil.h"
#endif

#ifdef MARATHON_RECOMP_METAL
#include "shader/msl/blend_color_alpha_ps.metal.metallib.h"
#include "shader/msl/copy_vs.metal.metallib.h"
#include "shader/msl/copy_color_ps.metal.metallib.h"
#include "shader/msl/copy_depth_ps.metal.metallib.h"
#include "shader/msl/csd_filter_ps.metal.metallib.h"
#include "shader/msl/csd_no_tex_vs.metal.metallib.h"
#include "shader/msl/csd_vs.metal.metallib.h"
#include "shader/msl/enhanced_motion_blur_ps.metal.metallib.h"
#include "shader/msl/gamma_correction_ps.metal.metallib.h"
#include "shader/msl/gaussian_blur_3x3.metal.metallib.h"
#include "shader/msl/gaussian_blur_5x5.metal.metallib.h"
#include "shader/msl/gaussian_blur_7x7.metal.metallib.h"
#include "shader/msl/gaussian_blur_9x9.metal.metallib.h"
#include "shader/msl/imgui_ps.metal.metallib.h"
#include "shader/msl/imgui_vs.metal.metallib.h"
#include "shader/msl/movie_ps.metal.metallib.h"
#include "shader/msl/movie_vs.metal.metallib.h"
#include "shader/msl/resolve_msaa_color_2x.metal.metallib.h"
#include "shader/msl/resolve_msaa_color_4x.metal.metallib.h"
#include "shader/msl/resolve_msaa_color_8x.metal.metallib.h"
#include "shader/msl/resolve_msaa_depth_2x.metal.metallib.h"
#include "shader/msl/resolve_msaa_depth_4x.metal.metallib.h"
#include "shader/msl/resolve_msaa_depth_8x.metal.metallib.h"
#endif

#include "shader/hlsl/blend_color_alpha_ps.hlsl.spirv.h"
#include "shader/hlsl/copy_vs.hlsl.spirv.h"
#include "shader/hlsl/copy_color_ps.hlsl.spirv.h"
#include "shader/hlsl/copy_depth_ps.hlsl.spirv.h"
#include "shader/hlsl/csd_filter_ps.hlsl.spirv.h"
#include "shader/hlsl/csd_no_tex_vs.hlsl.spirv.h"
#include "shader/hlsl/csd_vs.hlsl.spirv.h"
#include "shader/hlsl/enhanced_motion_blur_ps.hlsl.spirv.h"
#include "shader/hlsl/gamma_correction_ps.hlsl.spirv.h"
#include "shader/hlsl/gaussian_blur_3x3.hlsl.spirv.h"
#include "shader/hlsl/gaussian_blur_5x5.hlsl.spirv.h"
#include "shader/hlsl/gaussian_blur_7x7.hlsl.spirv.h"
#include "shader/hlsl/gaussian_blur_9x9.hlsl.spirv.h"
#include "shader/hlsl/imgui_ps.hlsl.spirv.h"
#include "shader/hlsl/imgui_vs.hlsl.spirv.h"
#include "shader/hlsl/movie_ps.hlsl.spirv.h"
#include "shader/hlsl/movie_vs.hlsl.spirv.h"
#include "shader/hlsl/resolve_msaa_color_2x.hlsl.spirv.h"
#include "shader/hlsl/resolve_msaa_color_4x.hlsl.spirv.h"
#include "shader/hlsl/resolve_msaa_color_8x.hlsl.spirv.h"
#include "shader/hlsl/resolve_msaa_depth_2x.hlsl.spirv.h"
#include "shader/hlsl/resolve_msaa_depth_4x.hlsl.spirv.h"
#include "shader/hlsl/resolve_msaa_depth_8x.hlsl.spirv.h"

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
#ifdef SDL_VULKAN_ENABLED
    extern std::unique_ptr<RenderInterface> CreateVulkanInterface(RenderWindow sdlWindow);
#else
    extern std::unique_ptr<RenderInterface> CreateVulkanInterface();
#endif

    static std::unique_ptr<RenderInterface> CreateVulkanInterfaceWrapper() {
#ifdef SDL_VULKAN_ENABLED
        return CreateVulkanInterface(GameWindow::s_renderWindow);
#else
        return CreateVulkanInterface();
#endif
    }
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
static Profiler g_updateDirectorProfiler;
static Profiler g_renderDirectorProfiler;
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

static std::unique_ptr<RenderCommandQueue> g_queue;
static std::unique_ptr<RenderCommandList> g_commandLists[NUM_FRAMES];
static std::unique_ptr<RenderCommandFence> g_commandFences[NUM_FRAMES];
static std::unique_ptr<RenderQueryPool> g_queryPools[NUM_FRAMES];
static bool g_commandListStates[NUM_FRAMES];

static Mutex g_copyMutex;
static std::unique_ptr<RenderCommandQueue> g_copyQueue;
static std::unique_ptr<RenderCommandList> g_copyCommandList;
static std::unique_ptr<RenderCommandFence> g_copyCommandFence;

static std::unique_ptr<RenderSwapChain> g_swapChain;
static bool g_swapChainValid;

static constexpr RenderFormat BACKBUFFER_FORMAT = RenderFormat::B8G8R8A8_UNORM;

static std::unique_ptr<RenderCommandSemaphore> g_acquireSemaphores[NUM_FRAMES];
static std::unique_ptr<RenderCommandSemaphore> g_renderSemaphores[NUM_FRAMES];
static uint32_t g_backBufferIndex;
static std::unique_ptr<GuestSurface> g_backBufferHolder;
static GuestSurface* g_backBuffer;

static std::unique_ptr<RenderTexture> g_intermediaryBackBufferTexture;
static uint32_t g_intermediaryBackBufferTextureWidth;
static uint32_t g_intermediaryBackBufferTextureHeight;
static uint32_t g_intermediaryBackBufferTextureDescriptorIndex;

static std::unique_ptr<RenderPipeline> g_gammaCorrectionPipeline;

struct std::unique_ptr<RenderDescriptorSet> g_textureDescriptorSet;
struct std::unique_ptr<RenderDescriptorSet> g_samplerDescriptorSet;

enum
{
    TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D,
    TEXTURE_DESCRIPTOR_NULL_TEXTURE_2D_ARRAY,
    TEXTURE_DESCRIPTOR_NULL_TEXTURE_CUBE,
    TEXTURE_DESCRIPTOR_NULL_COUNT
};

struct TextureDescriptorAllocator
{
    Mutex mutex;
    uint32_t capacity = TEXTURE_DESCRIPTOR_NULL_COUNT;
    std::vector<uint32_t> freed;

    uint32_t allocate()
    {
        std::lock_guard lock(mutex);

        uint32_t value;
        if (!freed.empty())
        {
            value = freed.back();
            freed.pop_back();
        }
        else
        {
            value = capacity;
            ++capacity;
        }

        return value;
    }

    void free(uint32_t value)
    {
        assert(value != NULL);
        std::lock_guard lock(mutex);
        freed.push_back(value);
    }
};

static std::unique_ptr<RenderTexture> g_blankTextures[TEXTURE_DESCRIPTOR_NULL_COUNT];
static std::unique_ptr<RenderTextureView> g_blankTextureViews[TEXTURE_DESCRIPTOR_NULL_COUNT];

static TextureDescriptorAllocator g_textureDescriptorAllocator;

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
    DatabaseData,
    PrecompilePipelines,
    RecompilePipelines
};

struct PipelineTask
{
    PipelineTaskType type{};
//    boost::shared_ptr<Hedgehog::Database::CDatabaseData> databaseData;
};

static Mutex g_pipelineTaskMutex;
static std::vector<PipelineTask> g_pipelineTaskQueue;

//static void EnqueuePipelineTask(PipelineTaskType type, const boost::shared_ptr<Hedgehog::Database::CDatabaseData>& databaseData)
//{
//    // Precompiled pipelines deliberately do not increment
//    // this counter to overlap the compilation with intro logos.
//    if (type != PipelineTaskType::PrecompilePipelines)
//        ++g_compilingPipelineTaskCount;
//
//    {
//        std::lock_guard lock(g_pipelineTaskMutex);
//        g_pipelineTaskQueue.emplace_back(type, databaseData);
//    }
//
//    if ((++g_pendingPipelineTaskCount) == 1)
//        g_pendingPipelineTaskCount.notify_one();
//}

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

struct IntermediaryUploadAllocator
{
    static constexpr size_t SIZE = 16 * 1024 * 1024;

    std::vector<std::unique_ptr<uint8_t[]>> buffers;
    uint32_t index = 0;
    uint32_t offset = 0;

    uint8_t* allocate(uint32_t size)
    {
        assert(size <= SIZE);

        if (offset + size > SIZE)
        {
            ++index;
            offset = 0;
        }

        if (buffers.size() <= index)
            buffers.resize(index + 1);

        auto& buffer = buffers[index];
        if (buffer == nullptr)
            buffer = std::make_unique_for_overwrite<uint8_t[]>(SIZE);

        auto result = buffer.get() + offset;
        offset += ((size + 0xF) & ~0xF);

        return result;
    }

    uint8_t* allocate(const void* memory, uint32_t size)
    {
        auto result = allocate(size);
        memcpy(result, memory, size);
        return result;
    }

    void reset()
    {
        index = 0;
        offset = 0;
    }
};

static IntermediaryUploadAllocator g_intermediaryUploadAllocator;

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

            if (surface->descriptorIndex != NULL)
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

static std::thread::id g_presentThreadId = std::this_thread::get_id();
static std::atomic<bool> g_readyForCommands;

// PPC_FUNC_IMPL(__imp__sub_824ECA00);
// PPC_FUNC(sub_824ECA00)
// {
//     g_readyForCommands.wait(false);
//     g_presentThreadId = std::this_thread::get_id();
//     __imp__sub_824ECA00(ctx, base);
// }

static ankerl::unordered_dense::map<RenderTexture*, RenderTextureLayout> g_barrierMap;

static void AddBarrier(GuestBaseTexture* texture, RenderTextureLayout layout)
{
    if (texture != nullptr && texture->layout != layout)
    {
        g_barrierMap[texture->texture] = layout;
        texture->layout = layout;
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
static std::unique_ptr<uint8_t[]> g_buttonBcDiff;

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

    g_buttonBcDiff = decompressZstd(g_button_bc_diff, g_button_bc_diff_uncompressed_size);
}

enum class CsdFilterState
{
    Unknown,
    On,
    Off
};

static CsdFilterState g_csdFilterState;

static ankerl::unordered_dense::set<GuestSurface*> g_pendingSurfaceCopies;
static ankerl::unordered_dense::set<GuestSurface*> g_pendingResolves;

enum class RenderCommandType
{
    SetRenderState,
    DestructResource,
    UnlockTextureRect,
    UnlockBuffer16,
    UnlockBuffer32,
    DrawImGui,
    ExecuteCommandList,
    BeginCommandList,
    StretchRect,
    SetRenderTarget,
    SetDepthStencilSurface,
    ExecutePendingStretchRectCommands,
    Clear,
    SetViewport,
    SetTexture,
    SetScissorRect,
    SetSamplerState,
    SetBooleans,
    SetVertexShaderConstants,
    SetPixelShaderConstants,
    AddPipeline,
    DrawPrimitive,
    DrawIndexedPrimitive,
    DrawPrimitiveUP,
    SetVertexDeclaration,
    SetVertexShader,
    SetStreamSource,
    SetIndices,
    SetPixelShader,
};

struct RenderCommand
{
    RenderCommandType type;
    union
    {
        struct
        {
            GuestRenderState type;
            uint32_t value;
        } setRenderState;

        struct 
        {
            GuestResource* resource;
        } destructResource;

        struct
        {
            GuestTexture* texture;
        } unlockTextureRect;

        struct
        {
            GuestBuffer* buffer;
        } unlockBuffer;

        struct 
        {
            GuestDevice* device;
            uint32_t flags;
            GuestTexture* texture;
            uint32_t destSliceOrFace;
        } stretchRect;

        struct 
        {
            GuestSurface* renderTarget;
        } setRenderTarget;

        struct 
        {
            GuestSurface* depthStencil;
        } setDepthStencilSurface;

        struct 
        {
            uint32_t flags;
            float color[4];
            float z;
            uint32_t stencil;
        } clear;

        struct 
        {
            float x;
            float y;
            float width;
            float height;
            float minDepth;
            float maxDepth;
        } setViewport;

        struct 
        {
            uint32_t index;
            GuestTexture* texture;
        } setTexture;

        struct 
        {
            int32_t left;
            int32_t top;
            int32_t right;
            int32_t bottom;
        } setScissorRect;

        struct
        {
            uint32_t index;
            uint32_t data0;
            uint32_t data3;
            uint32_t data5;
        } setSamplerState;

        struct
        {
            uint32_t booleans;
        } setBooleans;

        struct
        {
            uint8_t* memory;
            uint32_t index;
            uint32_t size;
        } setVertexShaderConstants;  
        
        struct
        {
            uint8_t* memory;
            uint32_t index;
            uint32_t size;
        } setPixelShaderConstants;

        struct
        {
            XXH64_hash_t hash;
            RenderPipeline* pipeline;
        } addPipeline;

        struct 
        {
            uint32_t primitiveType; 
            uint32_t startVertex; 
            uint32_t primitiveCount;
        } drawPrimitive;

        struct 
        {
            uint32_t primitiveType;
            int32_t baseVertexIndex; 
            uint32_t startIndex;
            uint32_t primCount;
        } drawIndexedPrimitive;

        struct 
        {
            uint32_t primitiveType;
            uint32_t primitiveCount; 
            uint8_t* vertexStreamZeroData;
            uint32_t vertexStreamZeroSize;
            uint32_t vertexStreamZeroStride;
            CsdFilterState csdFilterState;
        } drawPrimitiveUP;

        struct 
        {
            GuestVertexDeclaration* vertexDeclaration;
        } setVertexDeclaration;

        struct 
        {
            GuestShader* shader;
        } setVertexShader;

        struct 
        {
            uint32_t index;
            GuestBuffer* buffer;
            uint32_t offset;
            uint32_t stride;
        } setStreamSource;

        struct 
        {
            GuestBuffer* buffer;
        } setIndices;

        struct 
        {
            GuestShader* shader;
        } setPixelShader;
    };
};

static moodycamel::BlockingConcurrentQueue<RenderCommand> g_renderQueue;

template<GuestRenderState TType>
static void SetRenderState(GuestDevice* device, uint32_t value)
{
    RenderCommand cmd;
    cmd.type = RenderCommandType::SetRenderState;
    cmd.setRenderState.type = TType;
    cmd.setRenderState.value = value;
    g_renderQueue.enqueue(cmd);
}

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

static RenderBlend ConvertBlendMode(uint32_t blendMode)
{
    switch (blendMode)
    {
    case D3DBLEND_ZERO:
        return RenderBlend::ZERO;
    case D3DBLEND_ONE:
        return RenderBlend::ONE;
    case D3DBLEND_SRCCOLOR:
        return RenderBlend::SRC_COLOR;
    case D3DBLEND_INVSRCCOLOR:
        return RenderBlend::INV_SRC_COLOR;
    case D3DBLEND_SRCALPHA:
        return RenderBlend::SRC_ALPHA;
    case D3DBLEND_INVSRCALPHA:
        return RenderBlend::INV_SRC_ALPHA;
    case D3DBLEND_DESTCOLOR:
        return RenderBlend::DEST_COLOR;
    case D3DBLEND_INVDESTCOLOR:
        return RenderBlend::INV_DEST_COLOR;
    case D3DBLEND_DESTALPHA:
        return RenderBlend::DEST_ALPHA;
    case D3DBLEND_INVDESTALPHA:
        return RenderBlend::INV_DEST_ALPHA;
    default:
        assert(false && "Invalid blend mode");
        return RenderBlend::ZERO;
    }
}

static RenderBlendOperation ConvertBlendOp(uint32_t blendOp)
{
    switch (blendOp)
    {
    case D3DBLENDOP_ADD:
        return RenderBlendOperation::ADD;
    case D3DBLENDOP_SUBTRACT:
        return RenderBlendOperation::SUBTRACT;
    case D3DBLENDOP_REVSUBTRACT:
        return RenderBlendOperation::REV_SUBTRACT;
    case D3DBLENDOP_MIN:
        return RenderBlendOperation::MIN;
    case D3DBLENDOP_MAX:
        return RenderBlendOperation::MAX;
    default:
        assert(false && "Unknown blend operation");
        return RenderBlendOperation::ADD;
    }
}

static RenderComparisonFunction ConvertCompareFunc(uint32_t compareFunc)
{
    switch (compareFunc)
    {
    case D3DCMP_NEVER:
        return RenderComparisonFunction::NEVER;
    case D3DCMP_LESS:
        return RenderComparisonFunction::LESS;
    case D3DCMP_EQUAL:
        return RenderComparisonFunction::EQUAL;
    case D3DCMP_LESSEQUAL:
        return RenderComparisonFunction::LESS_EQUAL;
    case D3DCMP_GREATER:
        return RenderComparisonFunction::GREATER;
    case D3DCMP_NOTEQUAL:
        return RenderComparisonFunction::NOT_EQUAL;
    case D3DCMP_GREATEREQUAL:
        return RenderComparisonFunction::GREATER_EQUAL;
    case D3DCMP_ALWAYS:
        return RenderComparisonFunction::ALWAYS;
    default:
        assert(false && "Unknown comparison function");
        return RenderComparisonFunction::NEVER;
    }
}

static RenderStencilOp ConvertStencilOp(uint32_t stencilOp)
{
    switch (stencilOp)
    {
    case D3DSTENCILOP_KEEP:
        return RenderStencilOp::KEEP;
    case D3DSTENCILOP_ZERO:
        return RenderStencilOp::ZERO;
    case D3DSTENCILOP_REPLACE:
        return RenderStencilOp::REPLACE;
    case D3DSTENCILOP_INCRSAT:
        return RenderStencilOp::INCREMENT_AND_CLAMP;
    case D3DSTENCILOP_DECRSAT:
        return RenderStencilOp::DECREMENT_AND_CLAMP;
    case D3DSTENCILOP_INVERT:
        return RenderStencilOp::INVERT;
    case D3DSTENCILOP_INCR:
        return RenderStencilOp::INCREMENT_AND_WRAP;
    case D3DSTENCILOP_DECR:
        return RenderStencilOp::DECREMENT_AND_WRAP;
    default:
        assert(false && "Unknown stencil op");
        return RenderStencilOp::KEEP;
    }
}

static void ProcSetRenderState(const RenderCommand& cmd)
{
    uint32_t value = cmd.setRenderState.value;

    switch (cmd.setRenderState.type)
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
        SetDirtyValue(g_dirtyStates.pipelineState, g_sharedConstants.alphaThreshold, float(value) / 256.0f);
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

static const std::pair<GuestRenderState, PPCFunc*> g_setRenderStateFunctions[] =
{
    { D3DRS_ZENABLE, HostToGuestFunction<SetRenderState<D3DRS_ZENABLE>> },
    { D3DRS_ZWRITEENABLE, HostToGuestFunction<SetRenderState<D3DRS_ZWRITEENABLE>> },
    { D3DRS_ALPHATESTENABLE, HostToGuestFunction<SetRenderState<D3DRS_ALPHATESTENABLE>> },
    { D3DRS_SRCBLEND, HostToGuestFunction<SetRenderState<D3DRS_SRCBLEND>> },
    { D3DRS_DESTBLEND, HostToGuestFunction<SetRenderState<D3DRS_DESTBLEND>> },
    { D3DRS_CULLMODE, HostToGuestFunction<SetRenderState<D3DRS_CULLMODE>> },
    { D3DRS_ZFUNC, HostToGuestFunction<SetRenderState<D3DRS_ZFUNC>> },
    { D3DRS_ALPHAREF, HostToGuestFunction<SetRenderState<D3DRS_ALPHAREF>> },
    { D3DRS_ALPHABLENDENABLE, HostToGuestFunction<SetRenderState<D3DRS_ALPHABLENDENABLE>> },
    { D3DRS_BLENDOP, HostToGuestFunction<SetRenderState<D3DRS_BLENDOP>> },
    { D3DRS_SCISSORTESTENABLE, HostToGuestFunction<SetRenderState<D3DRS_SCISSORTESTENABLE>> },
    { D3DRS_SLOPESCALEDEPTHBIAS, HostToGuestFunction<SetRenderState<D3DRS_SLOPESCALEDEPTHBIAS>> },
    { D3DRS_DEPTHBIAS, HostToGuestFunction<SetRenderState<D3DRS_DEPTHBIAS>> },
    { D3DRS_SRCBLENDALPHA, HostToGuestFunction<SetRenderState<D3DRS_SRCBLENDALPHA>> },
    { D3DRS_DESTBLENDALPHA, HostToGuestFunction<SetRenderState<D3DRS_DESTBLENDALPHA>> },
    { D3DRS_BLENDOPALPHA, HostToGuestFunction<SetRenderState<D3DRS_BLENDOPALPHA>> },
    { D3DRS_COLORWRITEENABLE, HostToGuestFunction<SetRenderState<D3DRS_COLORWRITEENABLE>> },
    { D3DRS_STENCILENABLE, HostToGuestFunction<SetRenderState<D3DRS_STENCILENABLE>> },
    { D3DRS_TWOSIDEDSTENCILMODE, HostToGuestFunction<SetRenderState<D3DRS_TWOSIDEDSTENCILMODE>> },
    { D3DRS_STENCILFAIL, HostToGuestFunction<SetRenderState<D3DRS_STENCILFAIL>> },
    { D3DRS_STENCILZFAIL, HostToGuestFunction<SetRenderState<D3DRS_STENCILZFAIL>> },
    { D3DRS_STENCILPASS, HostToGuestFunction<SetRenderState<D3DRS_STENCILPASS>> },
    { D3DRS_STENCILFUNC, HostToGuestFunction<SetRenderState<D3DRS_STENCILFUNC>> },
    { D3DRS_STENCILREF, HostToGuestFunction<SetRenderState<D3DRS_STENCILREF>> },
    { D3DRS_STENCILMASK, HostToGuestFunction<SetRenderState<D3DRS_STENCILMASK>> },
    { D3DRS_STENCILWRITEMASK, HostToGuestFunction<SetRenderState<D3DRS_STENCILWRITEMASK>> },
    { D3DRS_CCW_STENCILFAIL, HostToGuestFunction<SetRenderState<D3DRS_CCW_STENCILFAIL>> },
    { D3DRS_CCW_STENCILZFAIL, HostToGuestFunction<SetRenderState<D3DRS_CCW_STENCILZFAIL>> },
    { D3DRS_CCW_STENCILPASS, HostToGuestFunction<SetRenderState<D3DRS_CCW_STENCILPASS>> },
    { D3DRS_CCW_STENCILFUNC, HostToGuestFunction<SetRenderState<D3DRS_CCW_STENCILFUNC>> },
    { D3DRS_CLIPPLANEENABLE, HostToGuestFunction<SetRenderState<D3DRS_CLIPPLANEENABLE>> }
};

static std::unique_ptr<RenderShader> g_copyShader;

static std::unique_ptr<RenderShader> g_copyColorShader;
static ankerl::unordered_dense::map<RenderFormat, std::unique_ptr<RenderPipeline>> g_copyColorPipelines;
static std::unique_ptr<RenderPipeline> g_copyDepthPipeline;

static std::unique_ptr<RenderShader> g_resolveMsaaColorShaders[3];
static ankerl::unordered_dense::map<RenderFormat, std::array<std::unique_ptr<RenderPipeline>, 3>> g_resolveMsaaColorPipelines;
static std::unique_ptr<RenderPipeline> g_resolveMsaaDepthPipelines[3];

enum
{
    GAUSSIAN_BLUR_3X3,
    GAUSSIAN_BLUR_5X5,
    GAUSSIAN_BLUR_7X7,
    GAUSSIAN_BLUR_9X9,
    GAUSSIAN_BLUR_COUNT
};

static std::unique_ptr<GuestShader> g_gaussianBlurShaders[GAUSSIAN_BLUR_COUNT];

static std::unique_ptr<GuestShader> g_csdFilterShader;
static GuestShader* g_csdShader;

static std::unique_ptr<GuestShader> g_enhancedMotionBlurShader;

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

static constexpr size_t TEXTURE_DESCRIPTOR_SIZE = 65536;
static constexpr size_t SAMPLER_DESCRIPTOR_SIZE = 1024;

static std::unique_ptr<GuestTexture> g_imFontTexture;
static std::unique_ptr<RenderPipelineLayout> g_imPipelineLayout;
static std::unique_ptr<RenderPipeline> g_imPipeline;
static std::unique_ptr<RenderPipeline> g_imAdditivePipeline;

template<typename T>
static void ExecuteCopyCommandList(const T& function)
{
    std::lock_guard lock(g_copyMutex);

    g_copyCommandList->begin();
    function();
    g_copyCommandList->end();
    g_copyQueue->executeCommandLists(g_copyCommandList.get(), g_copyCommandFence.get());
    g_copyQueue->waitForCommandFence(g_copyCommandFence.get());
}

static constexpr uint32_t PITCH_ALIGNMENT = 0x100;
static constexpr uint32_t PLACEMENT_ALIGNMENT = 0x200;

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

extern ImFontBuilderIO g_fontBuilderIO;

static void CreateImGuiBackend()
{
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

#ifdef ENABLE_IM_FONT_ATLAS_SNAPSHOT
    IM_DELETE(io.Fonts);
    io.Fonts = ImFontAtlasSnapshot::Load();
#else
    io.Fonts->AddFontDefault();
    ImFontAtlasSnapshot::GenerateGlyphRanges();
#endif

    InitImGuiUtils();
    AchievementMenu::Init();
    AchievementOverlay::Init();
    ButtonGuide::Init();
    MessageWindow::Init();
    OptionsMenu::Init();
    InstallerWizard::Init();

    ImGui_ImplSDL2_InitForOther(GameWindow::s_pWindow);

#ifdef ENABLE_IM_FONT_ATLAS_SNAPSHOT
    g_imFontTexture = LoadTexture(
        decompressZstd(g_im_font_atlas_texture, g_im_font_atlas_texture_uncompressed_size).get(), g_im_font_atlas_texture_uncompressed_size);
#else
    io.Fonts->FontBuilderIO = &g_fontBuilderIO;
    io.Fonts->Build();

    g_imFontTexture = std::make_unique<GuestTexture>(ResourceType::Texture);

    uint8_t* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    RenderTextureDesc textureDesc;
    textureDesc.dimension = RenderTextureDimension::TEXTURE_2D;
    textureDesc.width = width;
    textureDesc.height = height;
    textureDesc.depth = 1;
    textureDesc.mipLevels = 1;
    textureDesc.arraySize = 1;
    textureDesc.format = RenderFormat::R8G8B8A8_UNORM;

    g_imFontTexture->textureHolder = g_device->createTexture(textureDesc);
    g_imFontTexture->texture = g_imFontTexture->textureHolder.get();

    uint32_t rowPitch = (width * 4 + PITCH_ALIGNMENT - 1) & ~(PITCH_ALIGNMENT - 1);
    uint32_t slicePitch = (rowPitch * height + PLACEMENT_ALIGNMENT - 1) & ~(PLACEMENT_ALIGNMENT - 1);
    auto uploadBuffer = g_device->createBuffer(RenderBufferDesc::UploadBuffer(slicePitch));
    uint8_t* mappedMemory = reinterpret_cast<uint8_t*>(uploadBuffer->map());

    if (rowPitch == (width * 4))
    {
        memcpy(mappedMemory, pixels, slicePitch);
    }
    else
    {
        for (size_t i = 0; i < height; i++)
        {
            memcpy(mappedMemory, pixels, width * 4);
            pixels += width * 4;
            mappedMemory += rowPitch;
        }
    }

    uploadBuffer->unmap();

    ExecuteCopyCommandList([&]
        {
            g_copyCommandList->barriers(RenderBarrierStage::COPY, RenderTextureBarrier(g_imFontTexture->texture, RenderTextureLayout::COPY_DEST));

            g_copyCommandList->copyTextureRegion(
                RenderTextureCopyLocation::Subresource(g_imFontTexture->texture, 0),
                RenderTextureCopyLocation::PlacedFootprint(uploadBuffer.get(), RenderFormat::R8G8B8A8_UNORM, width, height, 1, rowPitch / 4, 0));
        });

    g_imFontTexture->layout = RenderTextureLayout::COPY_DEST;

    RenderTextureViewDesc textureViewDesc;
    textureViewDesc.format = textureDesc.format;
    textureViewDesc.dimension = RenderTextureViewDimension::TEXTURE_2D;
    textureViewDesc.mipLevels = 1;
    g_imFontTexture->textureView = g_imFontTexture->texture->createTextureView(textureViewDesc);

    g_imFontTexture->descriptorIndex = g_textureDescriptorAllocator.allocate();
    g_textureDescriptorSet->setTexture(g_imFontTexture->descriptorIndex, g_imFontTexture->texture, RenderTextureLayout::SHADER_READ, g_imFontTexture->textureView.get());
#endif

    io.Fonts->SetTexID(g_imFontTexture.get());

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

#ifndef ENABLE_IM_FONT_ATLAS_SNAPSHOT
    ImFontAtlasSnapshot snapshot;
    snapshot.Snap();

    FILE* file = fopen("im_font_atlas.bin", "wb");
    if (file)
    {
        fwrite(snapshot.data.data(), 1, snapshot.data.size(), file);
        fclose(file);
    }

    ddspp::Header header;
    ddspp::HeaderDXT10 headerDX10;
    ddspp::encode_header(ddspp::R8G8B8A8_UNORM, width, height, 1, ddspp::Texture2D, 1, 1, header, headerDX10);

    file = fopen("im_font_atlas.dds", "wb");
    if (file)
    {
        fwrite(&ddspp::DDS_MAGIC, 4, 1, file);
        fwrite(&header, sizeof(header), 1, file);
        fwrite(&headerDX10, sizeof(headerDX10), 1, file);
        fwrite(pixels, 4, width * height, file);
        fclose(file);
    }
#endif
}

static void CheckSwapChain()
{
    g_swapChain->setVsyncEnabled(Config::VSync);
    g_swapChainValid &= !g_swapChain->needsResize();

    if (!g_swapChainValid)
    {
        Video::WaitForGPU();
        g_backBuffer->framebuffers.clear();
        g_swapChainValid = g_swapChain->resize();
        g_needsResize = g_swapChainValid;
    }

    if (g_swapChainValid)
    {
        g_swapChainAcquireProfiler.Begin();
        g_swapChainValid = g_swapChain->acquireTexture(g_acquireSemaphores[g_frame].get(), &g_backBufferIndex);
        g_swapChainAcquireProfiler.End();
    }

    if (g_needsResize)
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
        }

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

    g_readyForCommands = true;
    g_readyForCommands.notify_one();
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

bool Video::CreateHostDevice(const char *sdlVideoDriver, bool graphicsApiRetry)
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
    using RenderInterfaceFunction = std::unique_ptr<RenderInterface>(void);
    std::vector<RenderInterfaceFunction *> interfaceFunctions;

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

    interfaceFunctions.push_back((g_backend == Backend::VULKAN) ? CreateVulkanInterfaceWrapper : CreateD3D12Interface);
    interfaceFunctions.push_back((g_backend == Backend::VULKAN) ? CreateD3D12Interface : CreateVulkanInterfaceWrapper);
#elif defined(MARATHON_RECOMP_METAL)
    interfaceFunctions.push_back((g_backend == Backend::VULKAN) ? CreateVulkanInterfaceWrapper : CreateMetalInterface);
    interfaceFunctions.push_back((g_backend == Backend::VULKAN) ? CreateMetalInterface : CreateVulkanInterfaceWrapper);
#else
    interfaceFunctions.push_back(CreateVulkanInterfaceWrapper);
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

                        if (deviceDescription.vendor == RenderDeviceVendor::AMD)
                        {
                            // AMD Drivers before this version have a known issue where MSAA resolve targets will fail to work correctly.
                            // If no specific graphics API was selected, we silently destroy this one and move to the next option as it'll
                            // just work incorrectly otherwise and result in visual glitches and 3D rendering not working in general.
                            constexpr uint64_t MinimumAMDDriverVersion = 0x1F00005DC2005CULL; // 31.0.24002.92
                            if ((Config::GraphicsAPI == EGraphicsAPI::Auto) && (deviceDescription.driverVersion < MinimumAMDDriverVersion))
                                redirectToVulkan = true;
                        }
                        else if (deviceDescription.vendor == RenderDeviceVendor::INTEL)
                        {
                            // Intel drivers on D3D12 are extremely buggy, introducing various graphical glitches.
                            // We will redirect users to Vulkan until a workaround can be found.
                            if (Config::GraphicsAPI == EGraphicsAPI::Auto)
                                redirectToVulkan = true;
                        }

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

                g_backend = (interfaceFunction == CreateVulkanInterfaceWrapper) ? Backend::VULKAN : Backend::D3D12;
#elif defined(MARATHON_RECOMP_METAL)
                g_backend = (interfaceFunction == CreateVulkanInterfaceWrapper) ? Backend::VULKAN : Backend::METAL;
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

    g_swapChain = g_queue->createSwapChain(GameWindow::s_renderWindow, bufferCount, BACKBUFFER_FORMAT, Config::MaxFrameLatency);
    g_swapChain->setVsyncEnabled(Config::VSync);
    g_swapChainValid = !g_swapChain->needsResize();

    for (auto& acquireSemaphore : g_acquireSemaphores)
        acquireSemaphore = g_device->createCommandSemaphore();
    
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

    for (auto& shader : g_gaussianBlurShaders)
        shader = std::make_unique<GuestShader>(ResourceType::PixelShader);

    g_gaussianBlurShaders[GAUSSIAN_BLUR_3X3]->shader = CREATE_SHADER(gaussian_blur_3x3);
    g_gaussianBlurShaders[GAUSSIAN_BLUR_5X5]->shader = CREATE_SHADER(gaussian_blur_5x5);
    g_gaussianBlurShaders[GAUSSIAN_BLUR_7X7]->shader = CREATE_SHADER(gaussian_blur_7x7);
    g_gaussianBlurShaders[GAUSSIAN_BLUR_9X9]->shader = CREATE_SHADER(gaussian_blur_9x9);

    g_csdFilterShader = std::make_unique<GuestShader>(ResourceType::PixelShader);
    g_csdFilterShader->shader = CREATE_SHADER(csd_filter_ps);

    g_enhancedMotionBlurShader = std::make_unique<GuestShader>(ResourceType::PixelShader);
    g_enhancedMotionBlurShader->shader = CREATE_SHADER(enhanced_motion_blur_ps);

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

static uint32_t getSetAddress(uint32_t base, int index) {
    uint32_t entryOffset = index * 0xC;
    uint32_t entryAddress = base + entryOffset;
    uint32_t setAddress = entryAddress + sizeof(uint32_t);
    return setAddress;
}

static uint32_t CreateDevice(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, be<uint32_t>* a6)
{
    LOGF_WARNING("{:p} {:p} {:p} {:p} {:p} {:p}\n", reinterpret_cast<void*>(a1), reinterpret_cast<void*>(a2), reinterpret_cast<void*>(a3), reinterpret_cast<void*>(a4), reinterpret_cast<void*>(a5), reinterpret_cast<void*>(a6));
    g_xdbfTextureCache = std::unordered_map<uint16_t, GuestTexture *>();

    // for (auto &achievement : g_xdbfWrapper.GetAchievements(XDBF_LANGUAGE_ENGLISH))
    // {
    //     // huh?
    //     if (!achievement.pImageBuffer || !achievement.ImageBufferSize)
    //         continue;

    //     g_xdbfTextureCache[achievement.ID] =
    //         LoadTexture((uint8_t *)achievement.pImageBuffer, achievement.ImageBufferSize).release();
    // }

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

    // InsertFucntion doesn't work, so we have to do this manually in the end
    for (auto& [state, function] : g_setRenderStateFunctions)
    {
        auto funcOffset = getSetAddress(functionOffset, state/4);
        uint32_t addr = __builtin_bswap32(*(uint32_t*)g_memory.Translate(funcOffset));
        printf("state %d of %x is %x\n", state, funcOffset, addr);
        g_memory.InsertFunction(addr, function);
        device->setRenderStateFunctions[state / 4] = addr;
    }

    for (size_t i = 0; i < std::size(device->setSamplerStateFunctions); i++)
        device->setSamplerStateFunctions[i] = *reinterpret_cast<uint32_t*>(g_memory.Translate(0x82B79CFC + i * 0xC));

    device->viewport.width = 1280.0f;
    device->viewport.height = 720.0f;
    device->viewport.maxZ = 1.0f;

    *a6 = g_memory.MapVirtual(device);

    return 0;
}

static void DestructResource(GuestResource* resource) 
{
    // Needed for hack in CreateSurface (remove if fix it)
    if (resource->type == ResourceType::RenderTarget || resource->type == ResourceType::DepthStencil)
    {
        const auto surface = reinterpret_cast<GuestSurface*>(resource);
        if (surface->wasCached) {
            return;
        }
    }
    RenderCommand cmd;
    cmd.type = RenderCommandType::DestructResource;
    cmd.destructResource.resource = resource;
    g_renderQueue.enqueue(cmd);
}

static void ProcDestructResource(const RenderCommand& cmd)
{
    const auto& args = cmd.destructResource;
    g_tempResources[g_frame].push_back(args.resource);
}

static uint32_t ComputeTexturePitch(GuestTexture* texture)
{
    return (texture->width * RenderFormatSize(texture->format) + PITCH_ALIGNMENT - 1) & ~(PITCH_ALIGNMENT - 1);
}

static void LockTextureRect(GuestTexture* texture, uint32_t, GuestLockedRect* lockedRect) 
{
    uint32_t pitch = ComputeTexturePitch(texture);
    uint32_t slicePitch = pitch * texture->height;

    if (texture->mappedMemory == nullptr)
        texture->mappedMemory = g_userHeap.AllocPhysical(slicePitch, 0x10);

    lockedRect->pitch = pitch;
    lockedRect->bits = g_memory.MapVirtual(texture->mappedMemory);
}

static void UnlockTextureRect(GuestTexture* texture) 
{
    assert(std::this_thread::get_id() == g_presentThreadId);

    RenderCommand cmd;
    cmd.type = RenderCommandType::UnlockTextureRect;
    cmd.unlockTextureRect.texture = texture;
    g_renderQueue.enqueue(cmd);
}

static void ProcUnlockTextureRect(const RenderCommand& cmd)
{
    const auto& args = cmd.unlockTextureRect;

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

static void* LockBuffer(GuestBuffer* buffer, uint32_t flags)
{
    buffer->lockedReadOnly = (flags & 0x10) != 0;

    if (buffer->mappedMemory == nullptr)
        buffer->mappedMemory = g_userHeap.AllocPhysical(buffer->dataSize, 0x10);

    return buffer->mappedMemory;
}

static void* LockVertexBuffer(GuestBuffer* buffer, uint32_t, uint32_t, uint32_t flags)
{
    return LockBuffer(buffer, flags);
}

static std::atomic<uint32_t> g_bufferUploadCount = 0;

template<typename T>
static void UnlockBuffer(GuestBuffer* buffer, bool useCopyQueue)
{
    auto copyBuffer = [&](T* dest)
        {
            auto src = reinterpret_cast<const T*>(buffer->mappedMemory);

            for (size_t i = 0; i < buffer->dataSize; i += sizeof(T))
            {
                *dest = ByteSwap(*src);
                ++dest;
                ++src;
            }
        };

    if (useCopyQueue && g_capabilities.gpuUploadHeap)
    {
        copyBuffer(reinterpret_cast<T*>(buffer->buffer->map()));
        buffer->buffer->unmap();
    }
    else
    {
        auto uploadBuffer = g_device->createBuffer(RenderBufferDesc::UploadBuffer(buffer->dataSize));
        copyBuffer(reinterpret_cast<T*>(uploadBuffer->map()));
        uploadBuffer->unmap();

        if (useCopyQueue)
        {
            ExecuteCopyCommandList([&]
                {
                    g_copyCommandList->copyBufferRegion(buffer->buffer->at(0), uploadBuffer->at(0), buffer->dataSize);
                });
        }
        else
        {
            auto& commandList = g_commandLists[g_frame];

            commandList->barriers(RenderBarrierStage::COPY, RenderBufferBarrier(buffer->buffer.get(), RenderBufferAccess::WRITE));
            commandList->copyBufferRegion(buffer->buffer->at(0), uploadBuffer->at(0), buffer->dataSize);
            commandList->barriers(RenderBarrierStage::GRAPHICS, RenderBufferBarrier(buffer->buffer.get(), RenderBufferAccess::READ));

            g_tempBuffers[g_frame].emplace_back(std::move(uploadBuffer));
        }
    }

    g_bufferUploadCount++;
}

template<typename T>
static void UnlockBuffer(GuestBuffer* buffer)
{
    if (!buffer->lockedReadOnly)
    {
        UnlockBuffer<T>(buffer, true);
    }
}

static void ProcUnlockBuffer16(const RenderCommand& cmd)
{
    UnlockBuffer<uint16_t>(cmd.unlockBuffer.buffer, false);
}

static void ProcUnlockBuffer32(const RenderCommand& cmd)
{
    UnlockBuffer<uint32_t>(cmd.unlockBuffer.buffer, false);
}

static void UnlockVertexBuffer(GuestBuffer* buffer)
{
    UnlockBuffer<uint32_t>(buffer);
}

static void GetVertexBufferDesc(GuestBuffer* buffer, GuestBufferDesc* desc) 
{
    desc->size = buffer->dataSize;
}

static void* LockIndexBuffer(GuestBuffer* buffer, uint32_t, uint32_t, uint32_t flags) 
{
    return LockBuffer(buffer, flags);
}

static void UnlockIndexBuffer(GuestBuffer* buffer) 
{
    if (buffer->guestFormat == D3DFMT_INDEX32)
        UnlockBuffer<uint32_t>(buffer);
    else
        UnlockBuffer<uint16_t>(buffer);
}

static void GetIndexBufferDesc(GuestBuffer* buffer, GuestBufferDesc* desc)
{
    desc->format = buffer->guestFormat;
    desc->size = buffer->dataSize;
}

static void GetSurfaceDesc(GuestSurface* surface, GuestSurfaceDesc* desc) 
{
    if (surface->width == 0 && surface->height == 0) {
        LOGF_WARNING("{:p} {:d} {:d} \n", reinterpret_cast<void*>(desc), surface->width, surface->height);
        __builtin_trap();
    }
    desc->width = surface->width;
    desc->height = surface->height;
    desc->format = surface->guestFormat;
    desc->type = 4; // D3DRTYPE_SURFACE
    // desc->multiSampleType = 0;
    if (surface->sampleCount == RenderSampleCount::COUNT_1) {
        desc->multiSampleType = 0;
    } else if (surface->sampleCount == RenderSampleCount::COUNT_2) {
        desc->multiSampleType = 1;
    } else {
        desc->multiSampleType = 2;
    }
    desc->multiSampleQuality = 0;
    desc->usage = 0;
}

static void GetVertexDeclaration(GuestVertexDeclaration* vertexDeclaration, GuestVertexElement* vertexElements, be<uint32_t>* count) 
{
    memcpy(vertexElements, vertexDeclaration->vertexElements.get(), vertexDeclaration->vertexElementCount * sizeof(GuestVertexElement));
    *count = vertexDeclaration->vertexElementCount;
}

static uint32_t HashVertexDeclaration(uint32_t vertexDeclaration) 
{
    // Vertex declarations are cached on host side, so the pointer itself can be used.
    return vertexDeclaration;
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
    bool toggleProfiler = SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_F1] != 0;

    if (!g_profilerWasToggled && toggleProfiler)
    {
        g_profilerVisible = !g_profilerVisible;

        GameWindow::SetFullscreenCursorVisibility(App::s_isInit ? g_profilerVisible : true);
    }

    g_profilerWasToggled = toggleProfiler;

    if (!g_profilerVisible)
        return;

    ImFont* font = ImFontAtlasSnapshot::GetFont("FOT-RodinPro-DB.otf");
    float defaultScale = font->Scale;
    font->Scale = ImGui::GetDefaultFont()->FontSize / font->FontSize;
    ImGui::PushFont(font);

    if (ImGui::Begin("Profiler", &g_profilerVisible))
    {
        g_applicationValues[g_profilerValueIndex] = App::s_deltaTime * 1000.0;

        const double applicationAvg = std::accumulate(g_applicationValues, g_applicationValues + PROFILER_VALUE_COUNT, 0.0) / PROFILER_VALUE_COUNT;
        double gpuFrameAvg = g_gpuFrameProfiler.UpdateAndReturnAverage();
        double presentAvg = g_presentProfiler.UpdateAndReturnAverage();
        double updateDirectorAvg = g_updateDirectorProfiler.UpdateAndReturnAverage();
        double renderDirectorAvg = g_renderDirectorProfiler.UpdateAndReturnAverage();
        double frameFenceAvg = g_frameFenceProfiler.UpdateAndReturnAverage();
        double presentWaitAvg = g_presentWaitProfiler.UpdateAndReturnAverage();
        double swapChainAcquireAvg = g_swapChainAcquireProfiler.UpdateAndReturnAverage();

        if (ImPlot::BeginPlot("Frame Time"))
        {
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 20.0);
            ImPlot::SetupAxis(ImAxis_Y1, "ms", ImPlotAxisFlags_None);
            ImPlot::PlotLine<double>("Application", g_applicationValues, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
            ImPlot::PlotLine<double>("GPU Frame", g_gpuFrameProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
            ImPlot::PlotLine<double>("Present", g_presentProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
            ImPlot::PlotLine<double>("Update Director", g_updateDirectorProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
            ImPlot::PlotLine<double>("Render Director", g_renderDirectorProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
            ImPlot::PlotLine<double>("Frame Fence", g_frameFenceProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
            ImPlot::PlotLine<double>("Present Wait", g_presentWaitProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
            ImPlot::PlotLine<double>("Swap Chain Acquire", g_swapChainAcquireProfiler.values, PROFILER_VALUE_COUNT, 1.0, 0.0, ImPlotLineFlags_None, g_profilerValueIndex);
            ImPlot::EndPlot();
        }

        g_profilerValueIndex = (g_profilerValueIndex + 1) % PROFILER_VALUE_COUNT;

        ImGui::Text("Current Application: %g ms (%g FPS)", App::s_deltaTime * 1000.0, 1.0 / App::s_deltaTime);
        ImGui::Text("Current GPU Frame: %g ms (%g FPS)", g_gpuFrameProfiler.value.load(), 1000.0 / g_gpuFrameProfiler.value.load());
        ImGui::Text("Current Present: %g ms (%g FPS)", g_presentProfiler.value.load(), 1000.0 / g_presentProfiler.value.load());
        ImGui::Text("Current Update Director: %g ms (%g FPS)", g_updateDirectorProfiler.value.load(), 1000.0 / g_updateDirectorProfiler.value.load());
        ImGui::Text("Current Render Director: %g ms (%g FPS)", g_renderDirectorProfiler.value.load(), 1000.0 / g_renderDirectorProfiler.value.load());
        ImGui::Text("Current Frame Fence: %g ms", g_frameFenceProfiler.value.load());
        ImGui::Text("Current Present Wait: %g ms", g_presentWaitProfiler.value.load());
        ImGui::Text("Current Swap Chain Acquire: %g ms", g_swapChainAcquireProfiler.value.load());

        ImGui::NewLine();

        ImGui::Text("Average Application: %g ms (%g FPS)", applicationAvg, 1000.0 / applicationAvg);
        ImGui::Text("Average GPU Frame: %g ms (%g FPS)", gpuFrameAvg, 1000.0 / gpuFrameAvg);
        ImGui::Text("Average Present: %g ms (%g FPS)", presentAvg, 1000.0 / presentAvg);
        ImGui::Text("Average Update Director: %g ms (%g FPS)", updateDirectorAvg, 1000.0 / updateDirectorAvg);
        ImGui::Text("Average Render Director: %g ms (%g FPS)", renderDirectorAvg, 1000.0 / renderDirectorAvg);
        ImGui::Text("Average Frame Fence: %g ms", frameFenceAvg);
        ImGui::Text("Average Present Wait: %g ms", presentWaitAvg);
        ImGui::Text("Average Swap Chain Acquire: %g ms", swapChainAcquireAvg);

        ImGui::NewLine();

        if (g_userHeap.heap != nullptr && g_userHeap.physicalHeap != nullptr)
        {
            O1HeapDiagnostics diagnostics, physicalDiagnostics;
            {
                std::lock_guard lock(g_userHeap.mutex);
                diagnostics = o1heapGetDiagnostics(g_userHeap.heap);
            }
            {
                std::lock_guard lock(g_userHeap.physicalMutex);
                physicalDiagnostics = o1heapGetDiagnostics(g_userHeap.physicalHeap);
            }

            ImGui::Text("Heap Allocated: %d MB", int32_t(diagnostics.allocated / (1024 * 1024)));
            ImGui::Text("Physical Heap Allocated: %d MB", int32_t(physicalDiagnostics.allocated / (1024 * 1024)));
        }

        ImGui::Text("GPU Waits: %d", int32_t(g_waitForGPUCount));
        ImGui::Text("Buffer Uploads: %d", int32_t(g_bufferUploadCount));
        ImGui::NewLine();

        ImGui::Text("Present Wait: %s", g_capabilities.presentWait ? "Supported" : "Unsupported");
        ImGui::Text("Triangle Fan: %s", g_capabilities.triangleFan ? "Supported" : "Unsupported");
        ImGui::Text("Dynamic Depth Bias: %s", g_capabilities.dynamicDepthBias ? "Supported" : "Unsupported");
        ImGui::Text("Hardware Resolve Modes: %s", g_capabilities.resolveModes ? "Supported" : "Unsupported");
        ImGui::Text("Triangle Strip Workaround: %s", g_triangleStripWorkaround ? "Enabled" : "Disabled");
        ImGui::NewLine();

        std::string backend;

        switch (g_backend) {
        case Backend::VULKAN:
            backend = "Vulkan";
            break;
        case Backend::D3D12:
            backend = "D3D12";
            break;
        case Backend::METAL:
            backend = "Metal";
            break;
        }

        ImGui::Text("API: %s", backend.c_str());
        ImGui::Text("Device: %s", g_device->getDescription().name.c_str());
        ImGui::Text("Device Type: %s", DeviceTypeName(g_device->getDescription().type));
        ImGui::Text("VRAM: %.2f MiB", (double)(g_device->getDescription().dedicatedVideoMemory) / (1024.0 * 1024.0));
        ImGui::Text("UMA: %s", g_capabilities.uma ? "Supported" : "Unsupported");
        ImGui::Text("GPU Upload Heap: %s", g_capabilities.gpuUploadHeap ? "Supported" : "Unsupported");

        const char* sdlVideoDriver = SDL_GetCurrentVideoDriver();
        if (sdlVideoDriver != nullptr)
            ImGui::Text("SDL Video Driver: %s", sdlVideoDriver);

        ImGui::NewLine();
        ImGui::Checkbox("Show FPS", &Config::ShowFPS.Value);
        ImGui::NewLine();

        if (ImGui::TreeNode("Device Names"))
        {
            ImGui::Indent();

            uint32_t deviceIndex = 0;
            for (const std::string &deviceName : g_interface->getDeviceNames())
            {
                ImGui::Text("Option #%d: %s", deviceIndex++, deviceName.c_str());
            }

            ImGui::Unindent();
            ImGui::TreePop();
        }
    }
    ImGui::End();

    ImGui::PopFont();
    font->Scale = defaultScale;
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
    auto font = ImFontAtlasSnapshot::GetFont("FOT-RodinPro-DB.otf");
    auto fontSize = Scale(10);
    auto textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, fmt.c_str());

    ImVec2 min = { Scale(40), Scale(30) };
    ImVec2 max = { min.x + std::max(Scale(75), textSize.x + Scale(10)), min.y + Scale(15) };
    ImVec2 textPos = { min.x + Scale(2), CENTRE_TEXT_VERT(min, max, textSize) + Scale(0.2f) };

    drawList->AddRectFilled(min, max, IM_COL32(0, 0, 0, 200));
    drawList->AddText(font, fontSize, textPos, IM_COL32_WHITE, fmt.c_str());
}

static void DrawImGui()
{
    ImGui_ImplSDL2_NewFrame();

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

#ifdef ASYNC_PSO_DEBUG
    if (ImGui::Begin("Async PSO Stats"))
    {
        ImGui::Text("Pipelines Created In Render Thread: %d", g_pipelinesCreatedInRenderThread.load());
        ImGui::Text("Pipelines Created Asynchronously: %d", g_pipelinesCreatedAsynchronously.load());
        ImGui::Text("Pipelines Dropped: %d", g_pipelinesDropped.load());
        ImGui::Text("Pipelines Currently Compiling: %d", g_pipelinesCurrentlyCompiling.load());
        ImGui::Text("Compiling Pipeline Task Count: %d", g_compilingPipelineTaskCount.load());
        ImGui::Text("Pending Pipeline Task Count: %d", g_pendingPipelineTaskCount.load());

        std::lock_guard lock(g_debugMutex);
        ImGui::TextUnformatted(g_pipelineDebugText.c_str());
    }
    ImGui::End();
#endif

    AchievementMenu::Draw();
//    OptionsMenu::Draw();
    AchievementOverlay::Draw();
    InstallerWizard::Draw();
    MessageWindow::Draw();
    ButtonGuide::Draw();
    Fader::Draw();
    BlackBar::Draw();

    assert(ImGui::GetBackgroundDrawList()->_ClipRectStack.Size == 1 && "Some clip rects were not removed from the stack!");

    DrawFPS();
    DrawProfiler();
    ImGui::Render();

    auto drawData = ImGui::GetDrawData();
    if (drawData->CmdListsCount != 0)
    {
        RenderCommand cmd;
        cmd.type = RenderCommandType::DrawImGui;
        g_renderQueue.enqueue(cmd);
    }
}

static void SetFramebuffer(GuestSurface *renderTarget, GuestSurface *depthStencil, bool settingForClear);

static void ProcDrawImGui(const RenderCommand& cmd)
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

                    if (texture == g_imFontTexture.get())
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

static bool g_shouldPrecompilePipelines;
static std::atomic<bool> g_executedCommandList;

void Video::Present() 
{
    g_readyForCommands = false;

    RenderCommand cmd;
    cmd.type = RenderCommandType::ExecutePendingStretchRectCommands;
    g_renderQueue.enqueue(cmd);

    DrawImGui();

    cmd.type = RenderCommandType::ExecuteCommandList;
    g_renderQueue.enqueue(cmd);

    // All the shaders are available at this point. We can precompile embedded PSOs then.
    if (g_shouldPrecompilePipelines)
    {
//        EnqueuePipelineTask(PipelineTaskType::PrecompilePipelines, {});
        g_shouldPrecompilePipelines = false;
    }

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

        RenderCommandSemaphore* signalSemaphores[] = { g_renderSemaphores[g_frame].get() };
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
    g_intermediaryUploadAllocator.reset();
    g_triangleFanIndexData.reset();
    g_quadIndexData.reset();

    CheckSwapChain();

    cmd.type = RenderCommandType::BeginCommandList;
    g_renderQueue.enqueue(cmd);

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

void Video::StartPipelinePrecompilation()
{
    g_shouldPrecompilePipelines = true;
}

static void SetRootDescriptor(const UploadAllocation& allocation, size_t index)
{
    auto& commandList = g_commandLists[g_frame];

    if (g_backend != Backend::D3D12)
        commandList->setGraphicsPushConstants(0, &allocation.deviceAddress, 8 * index, 8);
    else
        commandList->setGraphicsRootDescriptor(allocation.buffer->at(allocation.offset), index);
}

static void ProcExecuteCommandList(const RenderCommand& cmd)
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
        RenderCommandSemaphore *signalSemaphores[] = { g_renderSemaphores[g_frame].get() };

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

static void ProcBeginCommandList(const RenderCommand& cmd)
{
    DestructTempResources();
    BeginCommandList();
}

static GuestSurface* GetBackBuffer() 
{
    g_backBuffer->AddRef();
    return g_backBuffer;
}

static GuestSurface* GetDepthStencil() 
{
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
    case EAspectRatio::Wide:
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

    case EAspectRatio::Narrow:
    case EAspectRatio::OriginalNarrow:
    {
        if (aspectRatio > NARROW_ASPECT_RATIO)
        {
            s_viewportWidth = height * 4 / 3;
            s_viewportHeight = height;
        }
        else
        {
            s_viewportWidth = width;
            s_viewportHeight = width * 3 / 4;
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

static RenderFormat ConvertFormat(uint32_t format)
{
    switch (format)
    {
    case D3DFMT_A16B16G16R16F:
    case D3DFMT_A16B16G16R16F_2:
    case D3DFMT_A16B16G16R16F_EXPAND:
        return RenderFormat::R16G16B16A16_FLOAT;
    case D3DFMT_LIN_A8R8G8B8:
        return RenderFormat::B8G8R8A8_UNORM;
    case D3DFMT_A8B8G8R8:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_LE_X8R8G8B8:
        return RenderFormat::R8G8B8A8_UNORM;
    case D3DFMT_R32F:
        return RenderFormat::R32_FLOAT;
    case D3DFMT_D24FS8:
    case D3DFMT_D24S8:
        return RenderFormat::D32_FLOAT_S8_UINT;
    case D3DFMT_G16R16F:
    case D3DFMT_G16R16F_2:
        return RenderFormat::R16G16_FLOAT;
    case D3DFMT_INDEX16:
        return RenderFormat::R16_UINT;
    case D3DFMT_INDEX32:
        return RenderFormat::R32_UINT;
    case D3DFMT_A8:
    case D3DFMT_L8:
    case D3DFMT_L8_2:
        return RenderFormat::R8_UNORM;
    case D3DFMT_DXT1:
        return RenderFormat::BC1_UNORM;
    case D3DFMT_DXT4:
        return RenderFormat::BC3_UNORM;
    default:
        LOGF_WARNING("{:x}\n", format);
        assert(false && "Unknown format");
        return RenderFormat::R16G16B16A16_FLOAT;
    }
}

static GuestTexture* CreateTexture(uint32_t width, uint32_t height, uint32_t depth, uint32_t levels, uint32_t usage, uint32_t format, uint32_t pool, uint32_t type) 
{
    ResourceType resourceType;

    switch (type)
    {
    case 17:
        resourceType = ResourceType::VolumeTexture;
        break;
    case 19:
        resourceType = ResourceType::ArrayTexture;
        break;
    default:
        resourceType = ResourceType::Texture;
        break;
    }

    const auto texture = g_userHeap.AllocPhysical<GuestTexture>(resourceType);

    RenderTextureDesc desc;
    desc.dimension = texture->type == ResourceType::VolumeTexture ? RenderTextureDimension::TEXTURE_3D : RenderTextureDimension::TEXTURE_2D;
    desc.width = width;
    desc.height = height;
    desc.mipLevels = levels;
    desc.format = ConvertFormat(format);

    if (texture->type == ResourceType::ArrayTexture) {
        desc.arraySize = depth;
        desc.depth = 1;
    } else {
        desc.depth = depth;
        desc.arraySize = 1;
    }

    if (RenderFormatIsDepth(desc.format))
        desc.flags = RenderTextureFlag::DEPTH_TARGET;
    else if (usage != 0)
        desc.flags = RenderTextureFlag::RENDER_TARGET;
    else
        desc.flags = RenderTextureFlag::NONE;

    texture->textureHolder = g_device->createTexture(desc);
    texture->texture = texture->textureHolder.get();

    RenderTextureViewDesc viewDesc;
    viewDesc.format = desc.format;
    viewDesc.dimension = texture->type == ResourceType::VolumeTexture ? RenderTextureViewDimension::TEXTURE_3D : RenderTextureViewDimension::TEXTURE_2D;
    viewDesc.mipLevels = levels;

    switch (format)
    {
    case D3DFMT_D24FS8:
    case D3DFMT_D24S8:
    case D3DFMT_L8:
    case D3DFMT_L8_2:
        viewDesc.componentMapping = RenderComponentMapping(RenderSwizzle::R, RenderSwizzle::R, RenderSwizzle::R, RenderSwizzle::ONE);
        break;

    case D3DFMT_X8R8G8B8:
        viewDesc.componentMapping = RenderComponentMapping(RenderSwizzle::G, RenderSwizzle::B, RenderSwizzle::A, RenderSwizzle::ONE);
        break;
    }

    texture->textureView = texture->texture->createTextureView(viewDesc);

    texture->width = width;
    texture->height = height;
    texture->depth = depth;
    texture->format = desc.format;
    texture->mipLevels = viewDesc.mipLevels;
    texture->viewDimension = viewDesc.dimension;
    texture->descriptorIndex = g_textureDescriptorAllocator.allocate();

    g_textureDescriptorSet->setTexture(texture->descriptorIndex, texture->texture, RenderTextureLayout::SHADER_READ, texture->textureView.get());
   
#ifdef _DEBUG 
    texture->texture->setName(fmt::format("Texture {:X}", g_memory.MapVirtual(texture)));
#endif
    // printf("CreateTexture: w: %d, h: %d, depth: %d, levels: %d, usage: %d, format: %d, pool: %d, type: %d - %x\n", width, height, depth, levels, usage, format, pool, type, texture);
    return texture;
}

static RenderHeapType GetBufferHeapType()
{
    return g_capabilities.gpuUploadHeap ? RenderHeapType::GPU_UPLOAD : RenderHeapType::DEFAULT;
}

static GuestBuffer* CreateVertexBuffer(uint32_t length) 
{
    auto buffer = g_userHeap.AllocPhysical<GuestBuffer>(ResourceType::VertexBuffer);
    buffer->buffer = g_device->createBuffer(RenderBufferDesc::VertexBuffer(length, GetBufferHeapType(), RenderBufferFlag::INDEX));
    buffer->dataSize = length;
#ifdef _DEBUG 
    buffer->buffer->setName(fmt::format("Vertex Buffer {:X}", g_memory.MapVirtual(buffer)));
#endif
    return buffer;
}

static GuestBuffer* CreateIndexBuffer(uint32_t length, uint32_t, uint32_t format)
{
    auto buffer = g_userHeap.AllocPhysical<GuestBuffer>(ResourceType::IndexBuffer);
    buffer->buffer = g_device->createBuffer(RenderBufferDesc::IndexBuffer(length, GetBufferHeapType()));
    buffer->dataSize = length;
    buffer->format = ConvertFormat(format);
    buffer->guestFormat = format;
#ifdef _DEBUG 
    buffer->buffer->setName(fmt::format("Index Buffer {:X}", g_memory.MapVirtual(buffer)));
#endif
    return buffer;
}

static std::vector<std::pair<GuestSurface*, uint32_t>> g_surfaceCache;

// TODO: singleplayer (possibly) uses the same memory location in edram for hdr and fb0 surfaces, so we just remember who was created first and use that instead of creating a new one
static GuestSurface* CreateSurface(uint32_t width, uint32_t height, uint32_t format, uint32_t multiSample, GuestSurfaceCreateParams* params) 
{
    GuestSurface* surface = nullptr;
    uint32_t baseValue = params ? params->base.get() : -1;
    if (params) {
        for (auto& entry : g_surfaceCache) {
            GuestSurface* cachedSurface = entry.first;
            uint32_t cachedBase = entry.second;
            if (cachedSurface &&
                cachedSurface->width == width &&
                cachedSurface->height == height &&
                cachedSurface->guestFormat == format &&
                cachedBase == baseValue) {
                surface = cachedSurface;
                break;
            }
        }
    }
    if (!surface) {
        // printf("CreateSurface: w: %d, h: %d, f: %d, ms: %d\n", width, height, format, multiSample);
        RenderTextureDesc desc;
        desc.dimension = RenderTextureDimension::TEXTURE_2D;
        desc.width = width;
        desc.height = height;
        desc.depth = 1;
        desc.mipLevels = 1;
        desc.arraySize = 1;
        // desc.multisampling.sampleCount = multiSample != 0 && Config::AntiAliasing != EAntiAliasing::None ? int32_t(Config::AntiAliasing.Value) : RenderSampleCount::COUNT_1;
        if (multiSample == 0) {
            desc.multisampling.sampleCount = RenderSampleCount::COUNT_1;
        } else {
            desc.multisampling.sampleCount = multiSample == 1 ? RenderSampleCount::COUNT_2 : RenderSampleCount::COUNT_4;
        }
        desc.format = ConvertFormat(format);
        desc.flags = RenderFormatIsDepth(desc.format) ? RenderTextureFlag::DEPTH_TARGET : RenderTextureFlag::RENDER_TARGET;

        surface = g_userHeap.AllocPhysical<GuestSurface>(RenderFormatIsDepth(desc.format) ?
            ResourceType::DepthStencil : ResourceType::RenderTarget);

        surface->textureHolder = g_device->createTexture(desc);
        surface->texture = surface->textureHolder.get();
        surface->width = width;
        surface->height = height;
        surface->format = desc.format;
        surface->guestFormat = format;
        surface->sampleCount = desc.multisampling.sampleCount;

        RenderTextureViewDesc viewDesc;
        viewDesc.dimension = RenderTextureViewDimension::TEXTURE_2D;
        viewDesc.format = desc.format;
        viewDesc.mipLevels = 1;
        surface->textureView = surface->textureHolder->createTextureView(viewDesc);
        surface->descriptorIndex = g_textureDescriptorAllocator.allocate();
        g_textureDescriptorSet->setTexture(surface->descriptorIndex, surface->textureHolder.get(), RenderTextureLayout::SHADER_READ, surface->textureView.get());

    #ifdef _DEBUG 
        surface->texture->setName(fmt::format("{} {:X}", desc.flags & RenderTextureFlag::RENDER_TARGET ? "Render Target" : "Depth Stencil", g_memory.MapVirtual(surface)));
    #endif
        if (params) {
            surface->wasCached = true;
            g_surfaceCache.emplace_back(surface, baseValue);
        }
    }

    return surface;
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

static void StretchRect(GuestDevice* device, uint32_t flags, uint32_t, GuestTexture* texture, uint32_t, uint32_t, uint32_t destSliceOrFace)
{
    // printf("StretchRect %x\n", texture);
    RenderCommand cmd;
    cmd.type = RenderCommandType::StretchRect;
    cmd.stretchRect.flags = flags;
    cmd.stretchRect.texture = texture;
    cmd.stretchRect.destSliceOrFace = destSliceOrFace;
    g_renderQueue.enqueue(cmd);
}

static void SetTextureInRenderThread(uint32_t index, GuestTexture* texture);
static void SetSurface(uint32_t index, GuestSurface* surface);

static void ProcStretchRect(const RenderCommand& cmd)
{
    const auto& args = cmd.stretchRect;

    const bool isDepthStencil = (args.flags & 0x4) != 0;
    const auto surface = isDepthStencil ? g_depthStencil : g_renderTarget;

    // Erase previous pending command so it doesn't cause the texture to be overriden.
    if (args.texture->sourceSurface != nullptr)
        args.texture->sourceSurface->destinationTextures.erase(args.texture);

    args.texture->sourceSurface = surface;
    // printf("ProcStretchRect: surface - %x %x ? (%x : %x)\n", surface, isDepthStencil, g_depthStencil, g_renderTarget);
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

static void SetDefaultViewport(GuestDevice* device, GuestSurface* surface)
{
    if (surface != nullptr)
    {
        RenderCommand cmd;
        cmd.type = RenderCommandType::SetViewport;
        cmd.setViewport.x = 0.0f;
        cmd.setViewport.y = 0.0f;
        cmd.setViewport.width = float(surface->width);
        cmd.setViewport.height = float(surface->height);
        cmd.setViewport.minDepth = 0.0f;
        cmd.setViewport.maxDepth = 1.0f;
        g_renderQueue.enqueue(cmd);

        device->viewport.x = 0.0f;
        device->viewport.y = 0.0f;
        device->viewport.width = float(surface->width);
        device->viewport.height = float(surface->height);
        device->viewport.minZ = 0.0f;
        device->viewport.maxZ = 1.0f;
    }
}

static void SetRenderTarget(GuestDevice* device, uint32_t index, GuestSurface* renderTarget) 
{
    if (index == 0)
    {
        RenderCommand cmd;
        cmd.type = RenderCommandType::SetRenderTarget;
        cmd.setRenderTarget.renderTarget = renderTarget;
        g_renderQueue.enqueue(cmd);

        SetDefaultViewport(device, renderTarget);
    }
    else
    {
        // Multiple targets are not currently handled. Make sure any attempt to set them is nullptr.
        assert(renderTarget == nullptr);
    }
}

static void ProcSetRenderTarget(const RenderCommand& cmd)
{
    const auto& args = cmd.setRenderTarget;
    SetDirtyValue(g_dirtyStates.renderTargetAndDepthStencil, g_renderTarget, args.renderTarget);
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.renderTargetFormat, args.renderTarget != nullptr ? args.renderTarget->format : RenderFormat::UNKNOWN);
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.sampleCount, args.renderTarget != nullptr ? args.renderTarget->sampleCount : RenderSampleCount::COUNT_1);

    // When alpha to coverage is enabled, update the alpha test mode as it's dependent on sample count.
    SetAlphaTestMode((g_pipelineState.specConstants & (SPEC_CONSTANT_ALPHA_TEST | SPEC_CONSTANT_ALPHA_TO_COVERAGE)) != 0);
}

static void SetDepthStencilSurface(GuestDevice* device, GuestSurface* depthStencil) 
{
    RenderCommand cmd;
    cmd.type = RenderCommandType::SetDepthStencilSurface;
    cmd.setDepthStencilSurface.depthStencil = depthStencil;
    g_renderQueue.enqueue(cmd);

    SetDefaultViewport(device, depthStencil);
}

static void ProcSetDepthStencilSurface(const RenderCommand& cmd)
{
    const auto& args = cmd.setDepthStencilSurface;

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

static void ProcExecutePendingStretchRectCommands(const RenderCommand& cmd)
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
            auto& framebuffer = framebufferContainer->framebuffers[framebufferKey];

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

static void Clear(GuestDevice* device, uint32_t flags, uint32_t, be<float>* color, double z, uint32_t stencil)
{
    RenderCommand cmd;
    cmd.type = RenderCommandType::Clear;
    cmd.clear.flags = flags;
    cmd.clear.color[0] = color[0];
    cmd.clear.color[1] = color[1];
    cmd.clear.color[2] = color[2];
    cmd.clear.color[3] = color[3];
    cmd.clear.z = float(z);
    cmd.clear.stencil = stencil;
    g_renderQueue.enqueue(cmd);
}

static void ProcClear(const RenderCommand& cmd)
{
    const auto& args = cmd.clear;

    if (PopulateBarriersForStretchRect(g_renderTarget, g_depthStencil))
    {
        FlushBarriers();
        ExecutePendingStretchRectCommands(g_renderTarget, g_depthStencil);
    }

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

    if (g_renderTarget != nullptr && (args.flags & D3DCLEAR_TARGET) != 0)
    {
        if (!canClearInOnePass) {
            SetFramebuffer(g_renderTarget, nullptr, true);
        }

        commandList->clearColor(0, RenderColor(args.color[0], args.color[1], args.color[2], args.color[3]));
    }

    const bool clearDepth = (args.flags & D3DCLEAR_ZBUFFER) != 0;
    const bool clearStencil = (args.flags & D3DCLEAR_STENCIL) != 0;
    if (g_depthStencil != nullptr && (clearDepth || clearStencil))
    {
        if (!canClearInOnePass) {
            SetFramebuffer(nullptr, g_depthStencil, true);
        }

        commandList->clearDepthStencil(clearDepth, clearStencil, args.z, args.stencil);
    }
}

static void SetViewport(GuestDevice* device, GuestViewport* viewport)
{
    RenderCommand cmd;
    cmd.type = RenderCommandType::SetViewport;
    cmd.setViewport.x = viewport->x;
    cmd.setViewport.y = viewport->y;
    cmd.setViewport.width = viewport->width;
    cmd.setViewport.height = viewport->height;
    cmd.setViewport.minDepth = viewport->minZ;
    cmd.setViewport.maxDepth = viewport->maxZ;
    g_renderQueue.enqueue(cmd);

    device->viewport.x = float(viewport->x);
    device->viewport.y = float(viewport->y);
    device->viewport.width = float(viewport->width);
    device->viewport.height = float(viewport->height);
    device->viewport.minZ = viewport->minZ;
    device->viewport.maxZ = viewport->maxZ;
}

static void ProcSetViewport(const RenderCommand& cmd)
{
    const auto& args = cmd.setViewport;

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

static void SetTexture(GuestDevice* device, uint32_t index, GuestTexture* texture) 
{
    // printf("SetTexture: %x %d %x\n", device, index, texture);
    auto isPlayStation = Config::ControllerIcons == EControllerIcons::PlayStation;

    if (Config::ControllerIcons == EControllerIcons::Auto)
        isPlayStation = hid::g_inputDeviceController == hid::EInputDevice::PlayStation;

    if (isPlayStation && texture != nullptr && texture->patchedTexture != nullptr)
        texture = texture->patchedTexture.get();

    RenderCommand cmd;
    cmd.type = RenderCommandType::SetTexture;
    cmd.setTexture.index = index;
    cmd.setTexture.texture = texture;
    g_renderQueue.enqueue(cmd);
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

static void ProcSetTexture(const RenderCommand& cmd)
{
    const auto& args = cmd.setTexture;

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

static void SetScissorRect(GuestDevice* device, GuestRect* rect)
{
    RenderCommand cmd;
    cmd.type = RenderCommandType::SetScissorRect;
    cmd.setScissorRect.top = rect->top;
    cmd.setScissorRect.left = rect->left;
    cmd.setScissorRect.bottom = rect->bottom;
    cmd.setScissorRect.right = rect->right;
    g_renderQueue.enqueue(cmd);
}

static void ProcSetScissorRect(const RenderCommand& cmd)
{
    const auto& args = cmd.setScissorRect;

    SetDirtyValue<int32_t>(g_dirtyStates.scissorRect, g_scissorRect.top, args.top);
    SetDirtyValue<int32_t>(g_dirtyStates.scissorRect, g_scissorRect.left, args.left);
    SetDirtyValue<int32_t>(g_dirtyStates.scissorRect, g_scissorRect.bottom, args.bottom);
    SetDirtyValue<int32_t>(g_dirtyStates.scissorRect, g_scissorRect.right, args.right);
}

static RenderShader* GetOrLinkShader(GuestShader* guestShader, uint32_t specConstants)
{
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
    desc.pixelShader = pipelineState.pixelShader != nullptr ? GetOrLinkShader(pipelineState.pixelShader, pipelineState.specConstants) : nullptr;
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

    XXH64_hash_t hash = XXH3_64bits(&pipelineState, sizeof(pipelineState));
    auto& pipeline = g_pipelines[hash];
    if (pipeline == nullptr)
    {
        pipeline = CreateGraphicsPipeline(pipelineState);

#ifdef ASYNC_PSO_DEBUG
        bool loading = *SWA::SGlobals::ms_IsLoading;

        if (loading)
            ++g_pipelinesCreatedAsynchronously;
        else
            ++g_pipelinesCreatedInRenderThread;

        pipeline->setName(fmt::format("{} {} {} {:X}", loading ? "ASYNC" : "",
            pipelineState.vertexShader->name, pipelineState.pixelShader != nullptr ? pipelineState.pixelShader->name : "<none>", hash));
        
        if (!loading)
        {
            std::lock_guard lock(g_debugMutex);
            g_pipelineDebugText = fmt::format(
                "PipelineState {:X}:\n"
                "  vertexShader: {}\n"
                "  pixelShader: {}\n"
                "  vertexDeclaration: {:X}\n"
                "  zEnable: {}\n"
                "  zWriteEnable: {}\n"
                "  stencilEnable: {}\n"
                "  stencilTwoSided: {}\n"
                "  srcBlend: {}\n"
                "  destBlend: {}\n"
                "  cullMode: {}\n"
                "  frontFace: {}\n"
                "  zFunc: {}\n"
                "  stencilFunc: {}\n"
                "  stencilFail: {}\n"
                "  stencilZFail: {}\n"
                "  stencilPass: {}\n"
                "  stencilFuncCCW: {}\n"
                "  stencilFailCCW: {}\n"
                "  stencilZFailCCW: {}\n"
                "  stencilPassCCW: {}\n"
                "  stencilMask: {}\n"
                "  stencilWriteMask: {}\n"
                "  stencilRef: {}\n"
                "  alphaBlendEnable: {}\n"
                "  blendOp: {}\n"
                "  slopeScaledDepthBias: {}\n"
                "  depthBias: {}\n"
                "  srcBlendAlpha: {}\n"
                "  destBlendAlpha: {}\n"
                "  blendOpAlpha: {}\n"
                "  colorWriteEnable: {:X}\n"
                "  primitiveTopology: {}\n"
                "  vertexStrides[0]: {}\n"
                "  vertexStrides[1]: {}\n"
                "  vertexStrides[2]: {}\n"
                "  vertexStrides[3]: {}\n"
                "  renderTargetFormat: {}\n"
                "  depthStencilFormat: {}\n"
                "  sampleCount: {}\n"
                "  enableAlphaToCoverage: {}\n"
                "  specConstants: {:X}\n",
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

static RenderTextureAddressMode ConvertTextureAddressMode(size_t value)
{
    switch (value)
    {
    case D3DTADDRESS_WRAP:
        return RenderTextureAddressMode::WRAP;
    case D3DTADDRESS_MIRROR:
        return RenderTextureAddressMode::MIRROR;
    case D3DTADDRESS_CLAMP:
        return RenderTextureAddressMode::CLAMP;
    case D3DTADDRESS_MIRRORONCE:
        return RenderTextureAddressMode::MIRROR_ONCE;
    case D3DTADDRESS_BORDER:
        return RenderTextureAddressMode::BORDER;
    default:
        assert(false && "Unknown texture address mode");
        return RenderTextureAddressMode::UNKNOWN;
    }
}

static RenderFilter ConvertTextureFilter(uint32_t value)
{
    switch (value)
    {
    case D3DTEXF_POINT:
    case D3DTEXF_NONE:
        return RenderFilter::NEAREST;
    case D3DTEXF_LINEAR:
        return RenderFilter::LINEAR;
    default:
        assert(false && "Unknown texture filter");
        return RenderFilter::UNKNOWN;
    }
}

static RenderBorderColor ConvertBorderColor(uint32_t value)
{
    switch (value)
    {
    case 0:
        return RenderBorderColor::TRANSPARENT_BLACK;
    case 1:
        return RenderBorderColor::OPAQUE_WHITE;
    default:
        assert(false && "Unknown border color");
        return RenderBorderColor::UNKNOWN;
    }
}

struct LocalRenderCommandQueue
{
    RenderCommand commands[20];
    uint32_t count = 0;

    RenderCommand& enqueue()
    {
        assert(count < std::size(commands));
        return commands[count++];
    }

    void submit()
    {
        g_renderQueue.enqueue_bulk(commands, count);
    }
};

static void FlushRenderStateForMainThread(GuestDevice* device, LocalRenderCommandQueue& queue)
{
    constexpr size_t BOOL_MASK = 0x2ull;
    if ((device->dirtyFlags[3].get() & BOOL_MASK) != 0)
    {
        auto& cmd = queue.enqueue();
        cmd.type = RenderCommandType::SetBooleans;
        cmd.setBooleans.booleans = (device->vertexShaderBoolConstants[0].get() & 0xFF) | ((device->pixelShaderBoolConstants[0].get() & 0xFF) << 16);

        device->dirtyFlags[3] = device->dirtyFlags[3].get() & ~BOOL_MASK;
    }

    for (uint32_t i = 0; i < 16; i++)
    {
        const size_t mask = 0x8000000000000000ull >> (i + 20);
        if (device->dirtyFlags[2].get() & mask)
        {
            auto& cmd = queue.enqueue();
            cmd.type = RenderCommandType::SetSamplerState;
            cmd.setSamplerState.index = i;
            cmd.setSamplerState.data0 = device->samplerStates[i].data[0];
            cmd.setSamplerState.data3 = device->samplerStates[i].data[3];
            cmd.setSamplerState.data5 = device->samplerStates[i].data[5];

            device->dirtyFlags[2] = device->dirtyFlags[2].get() & ~mask;
        }
    }

    uint64_t dirtyFlags = device->dirtyFlags[0].get();
    if (dirtyFlags != 0)
    {
        int startRegister = std::countl_zero(dirtyFlags);
        int endRegister = 64 - std::countr_zero(dirtyFlags);

        uint32_t index = startRegister * 16;
        uint32_t size = (endRegister - startRegister) * 64;

        auto& cmd = queue.enqueue();
        cmd.type = RenderCommandType::SetVertexShaderConstants;
        cmd.setVertexShaderConstants.memory = g_intermediaryUploadAllocator.allocate(&device->vertexShaderFloatConstants[index], size);
        cmd.setVertexShaderConstants.index = index;
        cmd.setVertexShaderConstants.size = size;

        device->dirtyFlags[0] = 0;
    }

    dirtyFlags = device->dirtyFlags[1].get();
    if (dirtyFlags != 0)
    {
        int startRegister = std::countl_zero(dirtyFlags);
        int endRegister = std::min(56, 64 - std::countr_zero(dirtyFlags));

        uint32_t index = startRegister * 16;
        uint32_t size = (endRegister - startRegister) * 64;

        auto& cmd = queue.enqueue();
        cmd.type = RenderCommandType::SetPixelShaderConstants;
        cmd.setPixelShaderConstants.memory = g_intermediaryUploadAllocator.allocate(&device->pixelShaderFloatConstants[index], size);
        cmd.setPixelShaderConstants.index = index;
        cmd.setPixelShaderConstants.size = size;

        device->dirtyFlags[1] = 0;
    }
}

static void ProcSetBooleans(const RenderCommand& cmd)
{
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.booleans, cmd.setBooleans.booleans);
}

static void ProcSetSamplerState(const RenderCommand& cmd)
{
    const auto& args = cmd.setSamplerState;

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

static void ProcSetVertexShaderConstants(const RenderCommand& cmd)
{
    auto& args = cmd.setVertexShaderConstants;
    assert((args.index * sizeof(uint32_t) + args.size) <= sizeof(g_vertexShaderConstants));

    memcpy(&g_vertexShaderConstants[args.index], args.memory, args.size);
    g_dirtyStates.vertexShaderConstants = true;
}

static void ProcSetPixelShaderConstants(const RenderCommand& cmd)
{
    auto& args = cmd.setPixelShaderConstants;
    assert((args.index * sizeof(uint32_t) + args.size) <= sizeof(g_pixelShaderConstants));

    memcpy(&g_pixelShaderConstants[args.index], args.memory, args.size);
    g_dirtyStates.pixelShaderConstants = true;
}

static void ProcAddPipeline(const RenderCommand& cmd)
{
    auto& args = cmd.addPipeline;
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

    for (const auto surface : g_pendingResolves)
    {
        bool isDepthStencil = RenderFormatIsDepth(surface->format);
        foundAny |= PopulateBarriersForStretchRect(isDepthStencil ? nullptr : surface, isDepthStencil ? surface : nullptr);
    }

    if (foundAny)
    {
        FlushBarriers();
        ExecutePendingStretchRectCommands(renderTarget, depthStencil);

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

static RenderPrimitiveTopology ConvertPrimitiveType(uint32_t primitiveType)
{
    switch (primitiveType)
    {
    case D3DPT_POINTLIST:
        return RenderPrimitiveTopology::POINT_LIST;
    case D3DPT_LINELIST:
        return RenderPrimitiveTopology::LINE_LIST;
    case D3DPT_LINESTRIP:
        return RenderPrimitiveTopology::LINE_STRIP;
    case D3DPT_TRIANGLELIST:
    case D3DPT_QUADLIST:
        return RenderPrimitiveTopology::TRIANGLE_LIST;
    case D3DPT_TRIANGLESTRIP:
        return RenderPrimitiveTopology::TRIANGLE_STRIP;
    case D3DPT_TRIANGLEFAN:
        return g_capabilities.triangleFan ? RenderPrimitiveTopology::TRIANGLE_FAN : RenderPrimitiveTopology::TRIANGLE_LIST;
    default:
        assert(false && "Unknown primitive type");
        return RenderPrimitiveTopology::UNKNOWN;
    }
}

static void SetPrimitiveType(uint32_t primitiveType)
{
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.primitiveTopology, ConvertPrimitiveType(primitiveType));
}

static void DrawPrimitive(GuestDevice* device, uint32_t primitiveType, uint32_t startVertex, uint32_t primitiveCount) 
{
    LocalRenderCommandQueue queue;
    FlushRenderStateForMainThread(device, queue);

    auto& cmd = queue.enqueue();
    cmd.type = RenderCommandType::DrawPrimitive;
    cmd.drawPrimitive.primitiveType = primitiveType;
    cmd.drawPrimitive.startVertex = startVertex;
    cmd.drawPrimitive.primitiveCount = primitiveCount;

    queue.submit();
}

static void ProcDrawPrimitive(const RenderCommand& cmd)
{
    const auto& args = cmd.drawPrimitive;

    SetPrimitiveType(args.primitiveType);

    FlushRenderStateForRenderThread();

    auto& commandList = g_commandLists[g_frame];
    commandList->drawInstanced(args.primitiveCount, 1, args.startVertex, 0);
}

static void DrawIndexedPrimitive(GuestDevice* device, uint32_t primitiveType, int32_t baseVertexIndex, uint32_t startIndex, uint32_t primCount)
{
    LocalRenderCommandQueue queue;
    FlushRenderStateForMainThread(device, queue);

    auto& cmd = queue.enqueue();
    cmd.type = RenderCommandType::DrawIndexedPrimitive;
    cmd.drawIndexedPrimitive.primitiveType = primitiveType;
    cmd.drawIndexedPrimitive.baseVertexIndex = baseVertexIndex;
    cmd.drawIndexedPrimitive.startIndex = startIndex;
    cmd.drawIndexedPrimitive.primCount = primCount;

    queue.submit();
}

static void ProcDrawIndexedPrimitive(const RenderCommand& cmd)
{
    const auto& args = cmd.drawIndexedPrimitive;

    SetPrimitiveType(args.primitiveType);
    FlushRenderStateForRenderThread();

    g_commandLists[g_frame]->drawIndexedInstanced(args.primCount, 1, args.startIndex, args.baseVertexIndex, 0);
}

static void DrawPrimitiveUP(GuestDevice* device, uint32_t primitiveType, uint32_t primitiveCount, void* vertexStreamZeroData, uint32_t vertexStreamZeroStride)
{
    LocalRenderCommandQueue queue;
    FlushRenderStateForMainThread(device, queue);

    auto& cmd = queue.enqueue();
    cmd.type = RenderCommandType::DrawPrimitiveUP;
    cmd.drawPrimitiveUP.primitiveType = primitiveType;
    cmd.drawPrimitiveUP.primitiveCount = primitiveCount;
    cmd.drawPrimitiveUP.vertexStreamZeroData = g_intermediaryUploadAllocator.allocate(vertexStreamZeroData, primitiveCount * vertexStreamZeroStride);
    cmd.drawPrimitiveUP.vertexStreamZeroSize = primitiveCount * vertexStreamZeroStride;
    cmd.drawPrimitiveUP.vertexStreamZeroStride = vertexStreamZeroStride;
    cmd.drawPrimitiveUP.csdFilterState = g_csdFilterState;
    
    queue.submit();
}

static void ProcDrawPrimitiveUP(const RenderCommand& cmd)
{
    const auto& args = cmd.drawPrimitiveUP;

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

    if (args.csdFilterState != CsdFilterState::Unknown &&
        (g_pipelineState.pixelShader == g_csdShader || g_pipelineState.pixelShader == g_csdFilterShader.get()))
    {
        SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.pixelShader,
            args.csdFilterState == CsdFilterState::On ? g_csdFilterShader.get() : g_csdShader);
    }

    FlushRenderStateForRenderThread();

    if (indexCount != 0)
        g_commandLists[g_frame]->drawIndexedInstanced(indexCount, 1, 0, 0, 0);
    else
        g_commandLists[g_frame]->drawInstanced(args.primitiveCount, 1, 0, 0);
}

static const char* ConvertDeclUsage(uint32_t usage)
{
    switch (usage)
    {
    case D3DDECLUSAGE_POSITION:
        return "POSITION";
    case D3DDECLUSAGE_BLENDWEIGHT:
        return "BLENDWEIGHT";
    case D3DDECLUSAGE_BLENDINDICES:
        return "BLENDINDICES";
    case D3DDECLUSAGE_NORMAL:
        return "NORMAL";
    case D3DDECLUSAGE_PSIZE:
        return "PSIZE";
    case D3DDECLUSAGE_TEXCOORD:
        return "TEXCOORD";
    case D3DDECLUSAGE_TANGENT:
        return "TANGENT";
    case D3DDECLUSAGE_BINORMAL:
        return "BINORMAL";
    case D3DDECLUSAGE_TESSFACTOR:
        return "TESSFACTOR";
    case D3DDECLUSAGE_POSITIONT:
        return "POSITIONT";
    case D3DDECLUSAGE_COLOR:
        return "COLOR";
    case D3DDECLUSAGE_FOG:
        return "FOG";
    case D3DDECLUSAGE_DEPTH:
        return "DEPTH";
    case D3DDECLUSAGE_SAMPLE:
        return "SAMPLE";
    default:
        assert(false && "Unknown usage");
        return "UNKNOWN";
    }
}

static RenderFormat ConvertDeclType(uint32_t type)
{
    switch (type)
    {
    case D3DDECLTYPE_FLOAT1:
        return RenderFormat::R32_FLOAT;
    case D3DDECLTYPE_FLOAT2:
        return RenderFormat::R32G32_FLOAT;
    case D3DDECLTYPE_FLOAT3:
        return RenderFormat::R32G32B32_FLOAT;
    case D3DDECLTYPE_FLOAT4:
        return RenderFormat::R32G32B32A32_FLOAT;
    case D3DDECLTYPE_D3DCOLOR:
        return RenderFormat::B8G8R8A8_UNORM;
    case D3DDECLTYPE_UBYTE4:
    case D3DDECLTYPE_UBYTE4_2:
        return RenderFormat::R8G8B8A8_UINT;
    case D3DDECLTYPE_SHORT2:
        return RenderFormat::R16G16_SINT;
    case D3DDECLTYPE_SHORT4:
        return RenderFormat::R16G16B16A16_SINT;
    case D3DDECLTYPE_UBYTE4N:
    case D3DDECLTYPE_UBYTE4N_2:
        return RenderFormat::R8G8B8A8_UNORM;
    case D3DDECLTYPE_SHORT2N:
        return RenderFormat::R16G16_SNORM;
    case D3DDECLTYPE_SHORT4N:
        return RenderFormat::R16G16B16A16_SNORM;
    case D3DDECLTYPE_USHORT2N:
        return RenderFormat::R16G16_UNORM;
    case D3DDECLTYPE_USHORT4N:
        return RenderFormat::R16G16B16A16_UNORM;
    case D3DDECLTYPE_UINT1:
        return RenderFormat::R32_UINT;
    case D3DDECLTYPE_DEC3N_2:
    case D3DDECLTYPE_DEC3N_3:
        return RenderFormat::R32_UINT;
    case D3DDECLTYPE_FLOAT16_2:
        return RenderFormat::R16G16_FLOAT;
    case D3DDECLTYPE_FLOAT16_4:
        return RenderFormat::R16G16B16A16_FLOAT;
    default:
        assert(false && "Unknown type");
        return RenderFormat::UNKNOWN;
    }
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

static void SetVertexDeclaration(GuestDevice* device, GuestVertexDeclaration* vertexDeclaration) 
{
    RenderCommand cmd;
    cmd.type = RenderCommandType::SetVertexDeclaration;
    cmd.setVertexDeclaration.vertexDeclaration = vertexDeclaration;
    g_renderQueue.enqueue(cmd);

    device->vertexDeclaration = g_memory.MapVirtual(vertexDeclaration);
}

static void ProcSetVertexDeclaration(const RenderCommand& cmd)
{
    auto& args = cmd.setVertexDeclaration;

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

static GuestShader* CreateShader(const be<uint32_t>* function, ResourceType resourceType)
{
    XXH64_hash_t hash = XXH3_64bits(function, function[1] + function[2]);

    auto findResult = FindShaderCacheEntry(hash);
    GuestShader* shader = nullptr;

    if (findResult == nullptr) {
        LOGF_WARNING("Shader of function {:x} is not found by value: {:x}", reinterpret_cast<uintptr_t>(function), hash);
        LOG_WARNING("Perhaps the path to the required shader will be printed before this error");
        __builtin_trap();
    }
    if (findResult != nullptr)
    {
        if (findResult->guestShader == nullptr)
        {
            shader = g_userHeap.AllocPhysical<GuestShader>(resourceType);

            if (hash == 0x85ED723035ECF535)
                shader->shader = CREATE_SHADER(blend_color_alpha_ps);
            else if (hash == 0xB1086A4947A797DE)
                shader->shader = CREATE_SHADER(csd_no_tex_vs);
            else if (hash == 0xB4CAFC034A37C8A8)
                shader->shader = CREATE_SHADER(csd_vs);
            else
                shader->shaderCacheEntry = findResult;

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

    if (hash == 0x31173204A896098A)
        g_csdShader = shader;

    return shader;
}

static GuestShader* CreateVertexShader(const be<uint32_t>* function) 
{
    return CreateShader(function, ResourceType::VertexShader);
}

static void SetVertexShader(GuestDevice* device, GuestShader* shader)
{
    RenderCommand cmd;
    cmd.type = RenderCommandType::SetVertexShader;
    cmd.setVertexShader.shader = shader;
    g_renderQueue.enqueue(cmd);
}

static void ProcSetVertexShader(const RenderCommand& cmd)
{
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.vertexShader, cmd.setVertexShader.shader);
}

static void SetStreamSource(GuestDevice* device, uint32_t index, GuestBuffer* buffer, uint32_t offset, uint32_t stride) 
{
    RenderCommand cmd;
    cmd.type = RenderCommandType::SetStreamSource;
    cmd.setStreamSource.index = index;
    cmd.setStreamSource.buffer = buffer;
    cmd.setStreamSource.offset = offset;
    cmd.setStreamSource.stride = stride;
    g_renderQueue.enqueue(cmd);
}

static void ProcSetStreamSource(const RenderCommand& cmd)
{
    const auto& args = cmd.setStreamSource;

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

static void SetIndices(GuestDevice* device, GuestBuffer* buffer) 
{
    RenderCommand cmd;
    cmd.type = RenderCommandType::SetIndices;
    cmd.setIndices.buffer = buffer;
    g_renderQueue.enqueue(cmd);
}

static void ProcSetIndices(const RenderCommand& cmd)
{
    const auto& args = cmd.setIndices;

    SetDirtyValue(g_dirtyStates.indices, g_indexBufferView.buffer, args.buffer != nullptr ? args.buffer->buffer->at(0) : RenderBufferReference{});
    SetDirtyValue(g_dirtyStates.indices, g_indexBufferView.format, args.buffer != nullptr ? args.buffer->format : RenderFormat::R16_UINT);
    SetDirtyValue(g_dirtyStates.indices, g_indexBufferView.size, args.buffer != nullptr ? args.buffer->dataSize : 0u);
}

static GuestShader* CreatePixelShader(const be<uint32_t>* function)
{
    return CreateShader(function, ResourceType::PixelShader);
}

static void SetPixelShader(GuestDevice* device, GuestShader* shader)
{
    RenderCommand cmd;
    cmd.type = RenderCommandType::SetPixelShader;
    cmd.setPixelShader.shader = shader;
    g_renderQueue.enqueue(cmd);
}

static void ProcSetPixelShader(const RenderCommand& cmd)
{
    GuestShader* shader = cmd.setPixelShader.shader;
    SetDirtyValue(g_dirtyStates.pipelineState, g_pipelineState.pixelShader, shader);
}

static void SetClipPlane(GuestDevice* device, uint32_t index, const be<float>* plane)
{
    if (index != 0)
        return;

    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.clipPlane[0], plane[0].get());
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.clipPlane[1], plane[1].get());
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.clipPlane[2], plane[2].get());
    SetDirtyValue(g_dirtyStates.sharedConstants, g_sharedConstants.clipPlane[3], plane[3].get());
}

static std::thread g_renderThread([]
    {
#ifdef _WIN32
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
        GuestThread::SetThreadName(GetCurrentThreadId(), "Render Thread");
#endif

        RenderCommand commands[32];

        while (true)
        {
            size_t count = g_renderQueue.wait_dequeue_bulk(commands, std::size(commands));

            for (size_t i = 0; i < count; i++)
            {
                auto& cmd = commands[i];
                switch (cmd.type)
                {
                case RenderCommandType::SetRenderState:                    ProcSetRenderState(cmd); break;
                case RenderCommandType::DestructResource:                  ProcDestructResource(cmd); break;
                case RenderCommandType::UnlockTextureRect:                 ProcUnlockTextureRect(cmd); break;
                case RenderCommandType::UnlockBuffer16:                    ProcUnlockBuffer16(cmd); break;
                case RenderCommandType::UnlockBuffer32:                    ProcUnlockBuffer32(cmd); break;
                case RenderCommandType::DrawImGui:                         ProcDrawImGui(cmd); break;
                case RenderCommandType::ExecuteCommandList:                ProcExecuteCommandList(cmd); break;
                case RenderCommandType::BeginCommandList:                  ProcBeginCommandList(cmd); break;
                case RenderCommandType::StretchRect:                       ProcStretchRect(cmd); break;
                case RenderCommandType::SetRenderTarget:                   ProcSetRenderTarget(cmd); break;
                case RenderCommandType::SetDepthStencilSurface:            ProcSetDepthStencilSurface(cmd); break;
                case RenderCommandType::ExecutePendingStretchRectCommands: ProcExecutePendingStretchRectCommands(cmd); break;
                case RenderCommandType::Clear:                             ProcClear(cmd); break;
                case RenderCommandType::SetViewport:                       ProcSetViewport(cmd); break;
                case RenderCommandType::SetTexture:                        ProcSetTexture(cmd); break;
                case RenderCommandType::SetScissorRect:                    ProcSetScissorRect(cmd); break;
                case RenderCommandType::SetSamplerState:                   ProcSetSamplerState(cmd); break;
                case RenderCommandType::SetBooleans:                       ProcSetBooleans(cmd); break;
                case RenderCommandType::SetVertexShaderConstants:          ProcSetVertexShaderConstants(cmd); break;
                case RenderCommandType::SetPixelShaderConstants:           ProcSetPixelShaderConstants(cmd); break;
                case RenderCommandType::AddPipeline:                       ProcAddPipeline(cmd); break;
                case RenderCommandType::DrawPrimitive:                     ProcDrawPrimitive(cmd); break;
                case RenderCommandType::DrawIndexedPrimitive:              ProcDrawIndexedPrimitive(cmd); break;
                case RenderCommandType::DrawPrimitiveUP:                   ProcDrawPrimitiveUP(cmd); break;
                case RenderCommandType::SetVertexDeclaration:              ProcSetVertexDeclaration(cmd); break;
                case RenderCommandType::SetVertexShader:                   ProcSetVertexShader(cmd); break;
                case RenderCommandType::SetStreamSource:                   ProcSetStreamSource(cmd); break;
                case RenderCommandType::SetIndices:                        ProcSetIndices(cmd); break;
                case RenderCommandType::SetPixelShader:                    ProcSetPixelShader(cmd); break;
                default:                                                   assert(false && "Unrecognized render command type."); break;
                }
            }
        }
    });

static void D3DXFillTexture(GuestTexture* texture, uint32_t function, void* data)
{
    if (texture->width == 1 && texture->height == 1 && texture->format == RenderFormat::R8_UNORM && function == 0x82BA2150)
    {
        auto uploadBuffer = g_device->createBuffer(RenderBufferDesc::UploadBuffer(PLACEMENT_ALIGNMENT));

        uint8_t* mappedData = reinterpret_cast<uint8_t*>(uploadBuffer->map());
        *mappedData = 0xFF;
        uploadBuffer->unmap();

        ExecuteCopyCommandList([&]
            {
                g_copyCommandList->barriers(RenderBarrierStage::COPY, RenderTextureBarrier(texture->texture, RenderTextureLayout::COPY_DEST));

                g_copyCommandList->copyTextureRegion(
                    RenderTextureCopyLocation::Subresource(texture->texture, 0),
                    RenderTextureCopyLocation::PlacedFootprint(uploadBuffer.get(), texture->format, 1, 1, 1, PLACEMENT_ALIGNMENT, 0));
            });

        texture->layout = RenderTextureLayout::COPY_DEST;
    }
}

static void D3DXFillVolumeTexture(GuestTexture* texture, uint32_t function, void* data)
{
    uint32_t rowPitch0 = (texture->width * 4 + PITCH_ALIGNMENT - 1) & ~(PITCH_ALIGNMENT - 1);
    uint32_t slicePitch0 = (rowPitch0 * texture->height * texture->depth + PLACEMENT_ALIGNMENT - 1) & ~(PLACEMENT_ALIGNMENT - 1);

    uint32_t rowPitch1 = ((texture->width / 2) * 4 + PITCH_ALIGNMENT - 1) & ~(PITCH_ALIGNMENT - 1);
    uint32_t slicePitch1 = (rowPitch1 * (texture->height / 2) * (texture->depth / 2) + PLACEMENT_ALIGNMENT - 1) & ~(PLACEMENT_ALIGNMENT - 1);

    auto uploadBuffer = g_device->createBuffer(RenderBufferDesc::UploadBuffer(slicePitch0 + slicePitch1));
    uint8_t* mappedData = reinterpret_cast<uint8_t*>(uploadBuffer->map());

    thread_local std::vector<float> mipData;
    mipData.resize((texture->width / 2) * (texture->height / 2) * (texture->depth / 2) * 4);
    memset(mipData.data(), 0, mipData.size() * sizeof(float));

    for (size_t z = 0; z < texture->depth; z++)
    {
        for (size_t y = 0; y < texture->height; y++)
        {
            for (size_t x = 0; x < texture->width; x++)
            {
                auto dest = mappedData + z * rowPitch0 * texture->height + y * rowPitch0 + x * sizeof(uint32_t);
                size_t index = z * texture->width * texture->height + y * texture->width + x;
                size_t mipIndex = ((z / 2) * (texture->width / 2) * (texture->height / 2) + (y / 2) * (texture->width / 2) + x / 2) * 4;

                if (function == 0x82BC7820)
                {
                    auto src = reinterpret_cast<be<float>*>(data) + index * 4;

                    float r = static_cast<uint8_t>(src[0] * 255.0f);
                    float g = static_cast<uint8_t>(src[1] * 255.0f);
                    float b = static_cast<uint8_t>(src[2] * 255.0f);
                    float a = static_cast<uint8_t>(src[3] * 255.0f);

                    dest[0] = r;
                    dest[1] = g;
                    dest[2] = b;
                    dest[3] = a;

                    mipData[mipIndex + 0] += r;
                    mipData[mipIndex + 1] += g;
                    mipData[mipIndex + 2] += b;
                    mipData[mipIndex + 3] += a;
                }
                else if (function == 0x82BC78A8)
                {
                    auto src = reinterpret_cast<uint8_t*>(data) + index * 4;

                    dest[0] = src[3];
                    dest[1] = src[2];
                    dest[2] = src[1];
                    dest[3] = src[0];

                    mipData[mipIndex + 0] += src[3];
                    mipData[mipIndex + 1] += src[2];
                    mipData[mipIndex + 2] += src[1];
                    mipData[mipIndex + 3] += src[0];
                }
            }
        }
    }

    for (size_t z = 0; z < texture->depth / 2; z++)
    {
        for (size_t y = 0; y < texture->height / 2; y++)
        {
            for (size_t x = 0; x < texture->width / 2; x++)
            {
                auto dest = mappedData + slicePitch0 + z * rowPitch1 * (texture->height / 2) + y * rowPitch1 + x * sizeof(uint32_t);
                size_t index = (z * (texture->width / 2) * (texture->height / 2) + y * (texture->width / 2) + x) * 4;

                dest[0] = static_cast<uint8_t>(mipData[index + 0] / 8.0f);
                dest[1] = static_cast<uint8_t>(mipData[index + 1] / 8.0f);
                dest[2] = static_cast<uint8_t>(mipData[index + 2] / 8.0f);
                dest[3] = static_cast<uint8_t>(mipData[index + 3] / 8.0f);
            }
        }
    }

    uploadBuffer->unmap();

    ExecuteCopyCommandList([&]
        {
            g_copyCommandList->barriers(RenderBarrierStage::COPY, RenderTextureBarrier(texture->texture, RenderTextureLayout::COPY_DEST));

            g_copyCommandList->copyTextureRegion(
                RenderTextureCopyLocation::Subresource(texture->texture, 0),
                RenderTextureCopyLocation::PlacedFootprint(uploadBuffer.get(), texture->format, texture->width, texture->height, texture->depth, rowPitch0 / RenderFormatSize(texture->format), 0));

            g_copyCommandList->copyTextureRegion(
                RenderTextureCopyLocation::Subresource(texture->texture, 1),
                RenderTextureCopyLocation::PlacedFootprint(uploadBuffer.get(), texture->format, texture->width / 2, texture->height / 2, texture->depth / 2, rowPitch1 / RenderFormatSize(texture->format), slicePitch0));
        });

    texture->layout = RenderTextureLayout::COPY_DEST;
}

struct GuestPictureData
{
    be<uint32_t> vtable;
    uint8_t flags;
    be<uint32_t> name;
    be<uint32_t> texture;
    be<uint32_t> type;
};

union Bxty {
    char _Buf[16];
    xpointer<uint32_t> _Ptr;
};

struct BEString
{
    char alval[1];
    Bxty bx;
    be<uint32_t> size;
    uint32_t res;

    const char* c_str() {
        uint32_t len = size.get();
        if (len == 0) {
            return alval;
        } else if (len < 16) {
            return bx._Buf;
        } else {
            return reinterpret_cast<const char*>(bx._Ptr.get());
        }
    }
};

struct GuestMyTexture
{
    be<uint32_t> vtable; // 0x0
    be<uint32_t> field0x4; // 0x4
    be<uint32_t> regIndex; // 0x8
    BEString str1; // 0xC
    BEString str2; // 0x28
    BEString str3; // 0x44
    be<uint32_t> byte60; // 0x60
    be<uint32_t> texture; // 0x64
    be<uint32_t> Surface[6]; // 0x64
    be<uint32_t> width; // 0x80
    be<uint32_t> height; // 0x84
    be<uint32_t> graphicsDevice; // 0x88
    be<uint32_t> guestDevice; // 0x8C
};

static RenderTextureDimension ConvertTextureDimension(ddspp::TextureType type)
{
    switch (type) 
    {
    case ddspp::Texture1D:
        return RenderTextureDimension::TEXTURE_1D;
    case ddspp::Texture2D:
    case ddspp::Cubemap:
        return RenderTextureDimension::TEXTURE_2D;
    case ddspp::Texture3D:
        return RenderTextureDimension::TEXTURE_3D;
    default:
        assert(false && "Unknown texture type from DDS.");
        return RenderTextureDimension::UNKNOWN;
    }
}

static RenderTextureViewDimension ConvertTextureViewDimension(ddspp::TextureType type)
{
    switch (type)
    {
    case ddspp::Texture1D:
        return RenderTextureViewDimension::TEXTURE_1D;
    case ddspp::Texture2D:
        return RenderTextureViewDimension::TEXTURE_2D;
    case ddspp::Texture3D:
        return RenderTextureViewDimension::TEXTURE_3D;
    case ddspp::Cubemap:
        return RenderTextureViewDimension::TEXTURE_CUBE;
    default:
        assert(false && "Unknown texture type from DDS.");
        return RenderTextureViewDimension::UNKNOWN;
    }
}

static RenderFormat ConvertDXGIFormat(ddspp::DXGIFormat format) 
{
    switch (format)
    {
    case ddspp::R32G32B32A32_TYPELESS:
        return RenderFormat::R32G32B32A32_TYPELESS;
    case ddspp::R32G32B32A32_FLOAT:
        return RenderFormat::R32G32B32A32_FLOAT;
    case ddspp::R32G32B32A32_UINT:
        return RenderFormat::R32G32B32A32_UINT;
    case ddspp::R32G32B32A32_SINT:
        return RenderFormat::R32G32B32A32_SINT;
    case ddspp::R32G32B32_TYPELESS:
        return RenderFormat::R32G32B32_TYPELESS;
    case ddspp::R32G32B32_FLOAT:
        return RenderFormat::R32G32B32_FLOAT;
    case ddspp::R32G32B32_UINT:
        return RenderFormat::R32G32B32_UINT;
    case ddspp::R32G32B32_SINT:
        return RenderFormat::R32G32B32_SINT;
    case ddspp::R16G16B16A16_TYPELESS:
        return RenderFormat::R16G16B16A16_TYPELESS;
    case ddspp::R16G16B16A16_FLOAT:
        return RenderFormat::R16G16B16A16_FLOAT;
    case ddspp::R16G16B16A16_UNORM:
        return RenderFormat::R16G16B16A16_UNORM;
    case ddspp::R16G16B16A16_UINT:
        return RenderFormat::R16G16B16A16_UINT;
    case ddspp::R16G16B16A16_SNORM:
        return RenderFormat::R16G16B16A16_SNORM;
    case ddspp::R16G16B16A16_SINT:
        return RenderFormat::R16G16B16A16_SINT;
    case ddspp::R32G32_TYPELESS:
        return RenderFormat::R32G32_TYPELESS;
    case ddspp::R32G32_FLOAT:
        return RenderFormat::R32G32_FLOAT;
    case ddspp::R32G32_UINT:
        return RenderFormat::R32G32_UINT;
    case ddspp::R32G32_SINT:
        return RenderFormat::R32G32_SINT;
    case ddspp::R8G8B8A8_TYPELESS:
        return RenderFormat::R8G8B8A8_TYPELESS;
    case ddspp::R8G8B8A8_UNORM:
        return RenderFormat::R8G8B8A8_UNORM;
    case ddspp::R8G8B8A8_UINT:
        return RenderFormat::R8G8B8A8_UINT;
    case ddspp::R8G8B8A8_SNORM:
        return RenderFormat::R8G8B8A8_SNORM;
    case ddspp::R8G8B8A8_SINT:
        return RenderFormat::R8G8B8A8_SINT;
    case ddspp::B8G8R8A8_UNORM:
        return RenderFormat::B8G8R8A8_UNORM;
    case ddspp::B8G8R8X8_UNORM:
        return RenderFormat::B8G8R8A8_UNORM;   
    case ddspp::R16G16_TYPELESS:
        return RenderFormat::R16G16_TYPELESS;
    case ddspp::R16G16_FLOAT:
        return RenderFormat::R16G16_FLOAT;
    case ddspp::R16G16_UNORM:
        return RenderFormat::R16G16_UNORM;
    case ddspp::R16G16_UINT:
        return RenderFormat::R16G16_UINT;
    case ddspp::R16G16_SNORM:
        return RenderFormat::R16G16_SNORM;
    case ddspp::R16G16_SINT:
        return RenderFormat::R16G16_SINT;
    case ddspp::R32_TYPELESS:
        return RenderFormat::R32_TYPELESS;
    case ddspp::D32_FLOAT:
        return RenderFormat::D32_FLOAT;
    case ddspp::R32_FLOAT:
        return RenderFormat::R32_FLOAT;
    case ddspp::R32_UINT:
        return RenderFormat::R32_UINT;
    case ddspp::R32_SINT:
        return RenderFormat::R32_SINT;
    case ddspp::R8G8_TYPELESS:
        return RenderFormat::R8G8_TYPELESS;
    case ddspp::R8G8_UNORM:
        return RenderFormat::R8G8_UNORM;
    case ddspp::R8G8_UINT:
        return RenderFormat::R8G8_UINT;
    case ddspp::R8G8_SNORM:
        return RenderFormat::R8G8_SNORM;
    case ddspp::R8G8_SINT:
        return RenderFormat::R8G8_SINT;
    case ddspp::R16_TYPELESS:
        return RenderFormat::R16_TYPELESS;
    case ddspp::R16_FLOAT:
        return RenderFormat::R16_FLOAT;
    case ddspp::D16_UNORM:
        return RenderFormat::D16_UNORM;
    case ddspp::R16_UNORM:
        return RenderFormat::R16_UNORM;
    case ddspp::R16_UINT:
        return RenderFormat::R16_UINT;
    case ddspp::R16_SNORM:
        return RenderFormat::R16_SNORM;
    case ddspp::R16_SINT:
        return RenderFormat::R16_SINT;
    case ddspp::R8_TYPELESS:
        return RenderFormat::R8_TYPELESS;
    case ddspp::R8_UNORM:
    case ddspp::A8_UNORM:
        return RenderFormat::R8_UNORM;
    case ddspp::R8_UINT:
        return RenderFormat::R8_UINT;
    case ddspp::R8_SNORM:
        return RenderFormat::R8_SNORM;
    case ddspp::R8_SINT:
        return RenderFormat::R8_SINT;
    case ddspp::BC1_TYPELESS:
        return RenderFormat::BC1_TYPELESS;
    case ddspp::BC1_UNORM:
        return RenderFormat::BC1_UNORM;
    case ddspp::BC1_UNORM_SRGB:
        return RenderFormat::BC1_UNORM_SRGB;
    case ddspp::BC2_TYPELESS:
        return RenderFormat::BC2_TYPELESS;
    case ddspp::BC2_UNORM:
        return RenderFormat::BC2_UNORM;
    case ddspp::BC2_UNORM_SRGB:
        return RenderFormat::BC2_UNORM_SRGB;
    case ddspp::BC3_TYPELESS:
        return RenderFormat::BC3_TYPELESS;
    case ddspp::BC3_UNORM:
        return RenderFormat::BC3_UNORM;
    case ddspp::BC3_UNORM_SRGB:
        return RenderFormat::BC3_UNORM_SRGB;
    case ddspp::BC4_TYPELESS:
        return RenderFormat::BC4_TYPELESS;
    case ddspp::BC4_UNORM:
        return RenderFormat::BC4_UNORM;
    case ddspp::BC4_SNORM:
        return RenderFormat::BC4_SNORM;
    case ddspp::BC5_TYPELESS:
        return RenderFormat::BC5_TYPELESS;
    case ddspp::BC5_UNORM:
        return RenderFormat::BC5_UNORM;
    case ddspp::BC5_SNORM:
        return RenderFormat::BC5_SNORM;
    case ddspp::BC6H_TYPELESS:
        return RenderFormat::BC6H_TYPELESS;
    case ddspp::BC6H_UF16:
        return RenderFormat::BC6H_UF16;
    case ddspp::BC6H_SF16:
        return RenderFormat::BC6H_SF16;
    case ddspp::BC7_TYPELESS:
        return RenderFormat::BC7_TYPELESS;
    case ddspp::BC7_UNORM:
        return RenderFormat::BC7_UNORM;
    case ddspp::BC7_UNORM_SRGB:
        return RenderFormat::BC7_UNORM_SRGB;
    default:
        printf("format: %x\n", format);
        assert(false && "Unsupported format from DDS.");
        return RenderFormat::UNKNOWN;
    }
}

static bool LoadTexture(GuestTexture& texture, const uint8_t* data, size_t dataSize, RenderComponentMapping componentMapping)
{
    ddspp::Descriptor ddsDesc;
    if (ddspp::decode_header((unsigned char *)(data), ddsDesc) != ddspp::Error)
    {
        RenderTextureDesc desc;
        desc.dimension = ConvertTextureDimension(ddsDesc.type);
        desc.width = ddsDesc.width;
        desc.height = ddsDesc.height;
        desc.depth = ddsDesc.depth;
        desc.mipLevels = ddsDesc.numMips;
        desc.arraySize = ddsDesc.type == ddspp::TextureType::Cubemap ? ddsDesc.arraySize * 6 : ddsDesc.arraySize;
        desc.format = ConvertDXGIFormat(ddsDesc.format);
        desc.flags = ddsDesc.type == ddspp::TextureType::Cubemap ? RenderTextureFlag::CUBE : RenderTextureFlag::NONE;

        texture.textureHolder = g_device->createTexture(desc);
        texture.texture = texture.textureHolder.get();
        texture.layout = RenderTextureLayout::COPY_DEST;

        RenderTextureViewDesc viewDesc;
        viewDesc.format = desc.format;
        viewDesc.dimension = ConvertTextureViewDimension(ddsDesc.type);
        viewDesc.mipLevels = ddsDesc.numMips;

        if (ddsDesc.format == ddspp::A8_UNORM)
        {
            // Map A8_UNORM to R8_UNORM for compatability
            componentMapping = RenderComponentMapping(RenderSwizzle::ZERO, RenderSwizzle::ZERO, RenderSwizzle::ZERO, RenderSwizzle::R);
        }

        viewDesc.componentMapping = componentMapping;
        texture.textureView = texture.texture->createTextureView(viewDesc);
        texture.descriptorIndex = g_textureDescriptorAllocator.allocate();
        g_textureDescriptorSet->setTexture(texture.descriptorIndex, texture.texture, RenderTextureLayout::SHADER_READ, texture.textureView.get());

        texture.width = ddsDesc.width;
        texture.height = ddsDesc.height;
        texture.mipLevels = viewDesc.mipLevels;
        texture.viewDimension = viewDesc.dimension;

        struct Slice
        {
            uint32_t width;
            uint32_t height;
            uint32_t depth;
            uint32_t srcOffset;
            uint32_t dstOffset;
            uint32_t srcRowPitch;
            uint32_t dstRowPitch;
            uint32_t rowCount;
        };

        std::vector<Slice> slices;
        uint32_t curSrcOffset = 0;
        uint32_t curDstOffset = 0;

        for (uint32_t arraySlice = 0; arraySlice < desc.arraySize; arraySlice++)
        {
            for (uint32_t mipSlice = 0; mipSlice < ddsDesc.numMips; mipSlice++)
            {
                auto& slice = slices.emplace_back();

                slice.width = std::max(1u, ddsDesc.width >> mipSlice);
                slice.height = std::max(1u, ddsDesc.height >> mipSlice);
                slice.depth = std::max(1u, ddsDesc.depth >> mipSlice);
                slice.srcOffset = curSrcOffset;
                slice.dstOffset = curDstOffset;
                uint32_t rowPitch = ((slice.width + ddsDesc.blockWidth - 1) / ddsDesc.blockWidth) * ddsDesc.bitsPerPixelOrBlock;
                slice.srcRowPitch = (rowPitch + 7) / 8;
                slice.dstRowPitch = (slice.srcRowPitch + PITCH_ALIGNMENT - 1) & ~(PITCH_ALIGNMENT - 1);
                slice.rowCount = (slice.height + ddsDesc.blockHeight - 1) / ddsDesc.blockHeight;

                curSrcOffset += slice.srcRowPitch * slice.rowCount * slice.depth;
                curDstOffset += (slice.dstRowPitch * slice.rowCount * slice.depth + PLACEMENT_ALIGNMENT - 1) & ~(PLACEMENT_ALIGNMENT - 1);
            }
        }

        auto uploadBuffer = g_device->createBuffer(RenderBufferDesc::UploadBuffer(curDstOffset));
        uint8_t* mappedMemory = reinterpret_cast<uint8_t*>(uploadBuffer->map());

        for (auto& slice : slices)
        {
            const uint8_t* srcData = data + ddsDesc.headerSize + slice.srcOffset;
            uint8_t* dstData = mappedMemory + slice.dstOffset;

            if (slice.srcRowPitch == slice.dstRowPitch)
            {
                memcpy(dstData, srcData, slice.srcRowPitch * slice.rowCount * slice.depth);
            }
            else
            {
                for (size_t i = 0; i < slice.rowCount * slice.depth; i++)
                {
                    memcpy(dstData, srcData, slice.srcRowPitch);
                    srcData += slice.srcRowPitch;
                    dstData += slice.dstRowPitch;
                }
            }
        }

        uploadBuffer->unmap();

        ExecuteCopyCommandList([&]
            {
                g_copyCommandList->barriers(RenderBarrierStage::COPY, RenderTextureBarrier(texture.texture, RenderTextureLayout::COPY_DEST));

                for (size_t i = 0; i < slices.size(); i++)
                {
                    auto& slice = slices[i];

                    g_copyCommandList->copyTextureRegion(
                        RenderTextureCopyLocation::Subresource(texture.texture, i % desc.mipLevels, i / desc.mipLevels),
                        RenderTextureCopyLocation::PlacedFootprint(uploadBuffer.get(), desc.format, slice.width, slice.height, slice.depth, (slice.dstRowPitch * 8) / ddsDesc.bitsPerPixelOrBlock * ddsDesc.blockWidth, slice.dstOffset));
                }
            });

        return true;
    }
    else
    {
        int width, height;
        void* stbImage = stbi_load_from_memory(data, dataSize, &width, &height, nullptr, 4);

        if (stbImage != nullptr)
        {
            texture.textureHolder = g_device->createTexture(RenderTextureDesc::Texture2D(width, height, 1, RenderFormat::R8G8B8A8_UNORM));
            texture.texture = texture.textureHolder.get();
            texture.viewDimension = RenderTextureViewDimension::TEXTURE_2D;
            texture.layout = RenderTextureLayout::COPY_DEST;

            texture.descriptorIndex = g_textureDescriptorAllocator.allocate();
            g_textureDescriptorSet->setTexture(texture.descriptorIndex, texture.texture, RenderTextureLayout::SHADER_READ);

            uint32_t rowPitch = (width * 4 + PITCH_ALIGNMENT - 1) & ~(PITCH_ALIGNMENT - 1);
            uint32_t slicePitch = rowPitch * height;

            auto uploadBuffer = g_device->createBuffer(RenderBufferDesc::UploadBuffer(slicePitch));
            uint8_t* mappedMemory = reinterpret_cast<uint8_t*>(uploadBuffer->map());

            if (rowPitch == (width * 4))
            {
                memcpy(mappedMemory, stbImage, slicePitch);
            }
            else
            {
                auto data = reinterpret_cast<const uint8_t*>(stbImage);

                for (size_t i = 0; i < height; i++)
                {
                    memcpy(mappedMemory, data, width * 4);
                    data += width * 4;
                    mappedMemory += rowPitch;
                }
            }

            uploadBuffer->unmap();

            stbi_image_free(stbImage);

            ExecuteCopyCommandList([&]
                {
                    g_copyCommandList->barriers(RenderBarrierStage::COPY, RenderTextureBarrier(texture.texture, RenderTextureLayout::COPY_DEST));

                    g_copyCommandList->copyTextureRegion(
                        RenderTextureCopyLocation::Subresource(texture.texture, 0),
                        RenderTextureCopyLocation::PlacedFootprint(uploadBuffer.get(), RenderFormat::R8G8B8A8_UNORM, width, height, 1, rowPitch / 4, 0));
                });

            return true;
        }
    }

    return false;
}

std::unique_ptr<GuestTexture> LoadTexture(const uint8_t* data, size_t dataSize, RenderComponentMapping componentMapping)
{
    GuestTexture texture(ResourceType::Texture);

    if (LoadTexture(texture, data, dataSize, componentMapping))
        return std::make_unique<GuestTexture>(std::move(texture));

    return nullptr;
}

static void DiffPatchTexture(GuestTexture& texture, uint8_t* data, uint32_t dataSize)
{
    auto header = reinterpret_cast<BlockCompressionDiffPatchHeader*>(g_buttonBcDiff.get());
    auto entries = reinterpret_cast<BlockCompressionDiffPatchEntry*>(g_buttonBcDiff.get() + header->entriesOffset);
    auto end = entries + header->entryCount;
    
    XXH64_hash_t hash = XXH3_64bits(data, dataSize);
    auto findResult = std::lower_bound(entries, end, hash, [](BlockCompressionDiffPatchEntry& lhs, XXH64_hash_t rhs)
        {
            return lhs.hash < rhs;
        });

    if (findResult != end && findResult->hash == hash)
    {
        auto patch = reinterpret_cast<BlockCompressionDiffPatch*>(g_buttonBcDiff.get() + findResult->patchesOffset);
        for (size_t i = 0; i < findResult->patchCount; i++)
        {
            assert(patch->destinationOffset + patch->patchBytesSize <= dataSize);
            memcpy(data + patch->destinationOffset, g_buttonBcDiff.get() + patch->patchBytesOffset, patch->patchBytesSize);
            ++patch;
        }

        GuestTexture patchedTexture(ResourceType::Texture);
        if (LoadTexture(patchedTexture, data, dataSize, {}))
            texture.patchedTexture = std::make_unique<GuestTexture>(std::move(patchedTexture));
    }
}

static void MakePictureData(GuestMyTexture* pictureData, uint8_t* data, uint32_t dataSize)
{
    if (data != nullptr)
    {
        GuestTexture texture(ResourceType::Texture);

        if (LoadTexture(texture, data, dataSize, {}))
        {
#ifdef _DEBUG
            if (pictureData->str1.size.get() > 0) {
                texture.texture->setName(fmt::format("Texture {}", pictureData->str1.c_str()));
            }
#endif

            // DiffPatchTexture(texture, data, dataSize);

            pictureData->texture = g_memory.MapVirtual(g_userHeap.AllocPhysical<GuestTexture>(std::move(texture)));
            pictureData->width = texture.width;
            pictureData->height = texture.height;
        }
    }
}

void IndexBufferLengthMidAsmHook(PPCRegister& r3)
{
    r3.u64 *= 2;
}

void SetShadowResolutionMidAsmHook(PPCRegister& r11)
{
    auto res = (int32_t)Config::ShadowResolution.Value;

    if (res > 0)
        r11.u64 = res;
}

static void SetResolution(be<uint32_t>* device)
{
    Video::ComputeViewportDimensions();

    uint32_t width = uint32_t(round(Video::s_viewportWidth * Config::ResolutionScale));
    uint32_t height = uint32_t(round(Video::s_viewportHeight * Config::ResolutionScale));
    device[46] = width == 0 ? 880 : width;
    device[47] = height == 0 ? 720 : height;
}

// The game does some weird stuff to render targets if they are above 
// 1024x1024 resolution, setting this bool at address 20 seems to avoid all that.
// PPC_FUNC(sub_82E9F048)
// {
//     PPC_STORE_U8(ctx.r4.u32 + 20, 1);
//     PPC_STORE_U32(ctx.r4.u32 + 44, PPC_LOAD_U32(ctx.r4.u32 + 8)); // Width
//     PPC_STORE_U32(ctx.r4.u32 + 48, PPC_LOAD_U32(ctx.r4.u32 + 12)); // Height
// }

static GuestShader* g_movieVertexShader;
static GuestShader* g_moviePixelShaderHD;
static GuestShader* g_moviePixelShaderSD;
static GuestVertexDeclaration* g_movieVertexDeclaration;


// do we even need them?
static int ScreenShaderInit(be<uint32_t>* a1)
{
    // auto g_xvs_VertexShader = (unsigned char*)g_memory.Translate(0x82062778);
    // auto g_xps_I420PixelShaderSd = (unsigned char*)g_memory.Translate(0x82062868);
    // auto g_xps_I420PixelShaderHd = (unsigned char*)g_memory.Translate(0x82062A30);

    // __builtin_trap();
    if (g_movieVertexShader == nullptr)
    {
        g_movieVertexShader = g_userHeap.AllocPhysical<GuestShader>(ResourceType::VertexShader);
        // g_movieVertexShader->shader = CREATE_SHADER(movie_vs);
    }

    if (g_moviePixelShaderHD == nullptr)
    {
        g_moviePixelShaderHD = g_userHeap.AllocPhysical<GuestShader>(ResourceType::PixelShader);
        // g_moviePixelShaderHD->shader = CREATE_SHADER(movie_ps);
    }

    if (g_moviePixelShaderSD == nullptr)
    {
        g_moviePixelShaderSD = g_userHeap.AllocPhysical<GuestShader>(ResourceType::PixelShader);
        // g_moviePixelShaderSD->shader = CREATE_SHADER(movie_ps);
    }

    g_moviePixelShaderHD->AddRef();
    g_moviePixelShaderSD->AddRef();
    g_movieVertexShader->AddRef();
    a1[0x12] = g_memory.MapVirtual(g_movieVertexShader);
    a1[0x51] = g_memory.MapVirtual(g_moviePixelShaderHD);
    a1[0x52] = g_memory.MapVirtual(g_moviePixelShaderSD);
    return 0;
}

// Needed for correct clearing of index buffer
static bool IsSet() {
    return true;
}

void MovieRendererMidAsmHook(PPCRegister& r3)
{
    auto device = reinterpret_cast<GuestDevice*>(g_memory.Translate(r3.u32));

    // Force linear filtering & clamp addressing
    for (size_t i = 0; i < 3; i++)
    {
        device->samplerStates[i].data[0] = (device->samplerStates[i].data[0].get() & ~0x7fc00) | 0x24800;
        device->samplerStates[i].data[3] = (device->samplerStates[i].data[3].get() & ~0x1f80000) | 0x1280000;
    }

    device->dirtyFlags[3] = device->dirtyFlags[3].get() | 0xe0000000ull;
}

static PPCRegister g_r4;
static PPCRegister g_r5;

// CRenderDirectorFxPipeline::Initialize
// PPC_FUNC_IMPL(__imp__sub_8258C8A0);
// PPC_FUNC(sub_8258C8A0)
// {
//     g_r4 = ctx.r4;
//     g_r5 = ctx.r5;
//     __imp__sub_8258C8A0(ctx, base);
// }

// CRenderDirectorFxPipeline::Update
// PPC_FUNC_IMPL(__imp__sub_8258CAE0);
// PPC_FUNC(sub_8258CAE0)
// {
//     if (g_needsResize)
//     {
//         // Backup job values. These get modified by cutscenes, 
//         // and resizing will cause the values to be forgotten.
//         auto traverseFxJobs = [&]<typename TCallback>(const TCallback& callback)
//         {
//             uint32_t scheduler = PPC_LOAD_U32(ctx.r3.u32 + 0xE0);
//             if (scheduler != NULL)
//             {
//                 uint32_t member = PPC_LOAD_U32(scheduler + 0x8);
//                 if (member != NULL)
//                 {
//                     for (uint32_t it = PPC_LOAD_U32(member + 0x24); it != PPC_LOAD_U32(member + 0x28); it += 8)
//                     {
//                         uint32_t job = PPC_LOAD_U32(it);
//                         if (job != NULL)
//                             callback(job);
//                     }
//                 }
//             }
//         };

//         union JobValues
//         {
//             struct
//             {
//                 uint8_t field50[0x18];
//                 uint8_t field88;
//             } fade;

//             struct
//             {
//                 uint8_t camera[0x120];
//                 uint8_t field44;
//                 uint8_t fieldA0;
//             } shadowMap;
//         };

//         std::map<uint32_t, JobValues> jobValuesMap;
//         traverseFxJobs([&](uint32_t job)
//             {
//                 uint32_t vfTable = PPC_LOAD_U32(job);

//                 if (vfTable == 0x820CA6F8) // SWA::CFxFade
//                 {
//                     // NOTE: Intentionally not storing shared pointers here. 
//                     // Game sends messages that assign these every frame already.
//                     JobValues jobValues{};

//                     memcpy(jobValues.fade.field50, base + job + 0x50, sizeof(jobValues.fade.field50));
//                     jobValues.fade.field88 = PPC_LOAD_U8(job + 0x88);

//                     jobValuesMap.emplace(PPC_LOAD_U32(job + 0x48), jobValues);
//                 }
//                 else if (vfTable == 0x820CAC5C) // SWA::CFxShadowMap
//                 {
//                     for (uint32_t it = PPC_LOAD_U32(job + 0x88); it != PPC_LOAD_U32(job + 0x8C); it += 8)
//                     {
//                         uint32_t camera = PPC_LOAD_U32(it);
//                         if (camera != NULL && PPC_LOAD_U32(camera) == 0x820BF83C) // SWA::CShadowMapCameraLiSPSM
//                         {
//                             JobValues jobValues{};

//                             memcpy(jobValues.shadowMap.camera, base + camera, sizeof(jobValues.shadowMap.camera));
//                             jobValues.shadowMap.field44 = PPC_LOAD_U8(job + 0x44);
//                             jobValues.shadowMap.fieldA0 = PPC_LOAD_U8(job + 0xA0);

//                             jobValuesMap.emplace(vfTable, jobValues);
//                             break;
//                         }
//                     }
//                 }
//             });

//         auto r3 = ctx.r3;
//         ctx.r4 = g_r4;
//         ctx.r5 = g_r5;
//         __imp__sub_8258C8A0(ctx, base);
//         ctx.r3 = r3;

//         // Restore job values.
//         traverseFxJobs([&](uint32_t job)
//             {
//                 uint32_t vfTable = PPC_LOAD_U32(job);

//                 if (vfTable == 0x820CA6F8) // SWA::CFxFade
//                 {
//                     auto findResult = jobValuesMap.find(PPC_LOAD_U32(job + 0x48));
//                     if (findResult != jobValuesMap.end()) // May NOT actually be found.
//                     {
//                         memcpy(base + job + 0x50, findResult->second.fade.field50, sizeof(findResult->second.fade.field50));
//                         PPC_STORE_U8(job + 0x88, findResult->second.fade.field88);
//                     }
//                 }
//                 else if (vfTable == 0x820CAC5C) // SWA::CFxShadowMap
//                 {
//                     auto findResult = jobValuesMap.find(vfTable);
//                     if (findResult != jobValuesMap.end()) // Would be weird if this one wasn't found.
//                     {
//                         for (uint32_t it = PPC_LOAD_U32(job + 0x88); it != PPC_LOAD_U32(job + 0x8C); it += 8)
//                         {
//                             uint32_t camera = PPC_LOAD_U32(it);
//                             if (camera != NULL && PPC_LOAD_U32(camera) == 0x820BF83C) // SWA::CShadowMapCameraLiSPSM
//                             {
//                                 memcpy(base + camera, findResult->second.shadowMap.camera, sizeof(findResult->second.shadowMap.camera));
//                                 PPC_STORE_U32(job + 0x80, camera);
//                                 PPC_STORE_U8(job + 0x44, findResult->second.shadowMap.field44);
//                                 PPC_STORE_U8(job + 0xA0, findResult->second.shadowMap.fieldA0);
//                                 break;
//                             }
//                         }
//                     }
//                 }
//             });

//         g_needsResize = false;
//     }

//     __imp__sub_8258CAE0(ctx, base);
// }

// PPC_FUNC_IMPL(__imp__sub_824EB5B0);
// PPC_FUNC(sub_824EB5B0)
// {
//     g_updateDirectorProfiler.Begin();
//     __imp__sub_824EB5B0(ctx, base);
//     g_updateDirectorProfiler.End();
// }

// PPC_FUNC_IMPL(__imp__sub_824EB290);
// PPC_FUNC(sub_824EB290)
// {
//     g_renderDirectorProfiler.Begin();
//     __imp__sub_824EB290(ctx, base);
//     g_renderDirectorProfiler.End();
// }

// World map disables VERT+, so scaling by width does not work for it.
static uint32_t g_forceCheckHeightForPostProcessFix;

// SWA::CWorldMapCamera::CWorldMapCamera
// PPC_FUNC_IMPL(__imp__sub_824860E0);
// PPC_FUNC(sub_824860E0)
// {
//     ++g_forceCheckHeightForPostProcessFix;
//     __imp__sub_824860E0(ctx, base);
// }

// SWA::CCameraController::~CCameraController
// PPC_FUNC_IMPL(__imp__sub_824831D0);
// PPC_FUNC(sub_824831D0)
// {
//     if (PPC_LOAD_U32(ctx.r3.u32) == 0x8202BF1C) // SWA::CWorldMapCamera
//         --g_forceCheckHeightForPostProcessFix;

//     __imp__sub_824831D0(ctx, base);
// }

void PostProcessResolutionFix(PPCRegister& r4, PPCRegister& f1, PPCRegister& f2)
{
    auto device = reinterpret_cast<be<uint32_t>*>(g_memory.Translate(r4.u32));

    uint32_t width = device[46].get();
    uint32_t height = device[47].get();
    double aspectRatio = double(width) / double(height);

    double factor;
    if ((aspectRatio >= WIDE_ASPECT_RATIO) || (g_forceCheckHeightForPostProcessFix != 0))
        factor = 720.0 / double(height);
    else
        factor = 1280.0 / double(width);

    f1.f64 *= factor;
    f2.f64 *= factor;
}

void LightShaftAspectRatioFix(PPCRegister& f28, PPCRegister& f0)
{
    f28.f64 = f0.f64;
}

static const be<uint16_t> g_particleTestIndexBuffer[] =
{
    0, 1, 2,
    0, 2, 3,
    0, 3, 4,
    0, 4, 5
};

bool ParticleTestIndexBufferMidAsmHook(PPCRegister& r30)
{
    if (!g_capabilities.triangleFan)
    {
        auto buffer = CreateIndexBuffer(sizeof(g_particleTestIndexBuffer), 0, D3DFMT_INDEX16);
        void* memory = LockIndexBuffer(buffer, 0, 0, 0);
        memcpy(memory, g_particleTestIndexBuffer, sizeof(g_particleTestIndexBuffer));
        UnlockIndexBuffer(buffer);

        r30.u32 = g_memory.MapVirtual(buffer);
        return true;
    }
    return false;
}

void ParticleTestDrawIndexedPrimitiveMidAsmHook(PPCRegister& r7)
{
    if (!g_capabilities.triangleFan)
        r7.u64 = std::size(g_particleTestIndexBuffer);
}

void MotionBlurPrevInvViewProjectionMidAsmHook(PPCRegister& r10)
{
    auto mtxProjection = reinterpret_cast<be<float>*>(g_memory.Translate(r10.u32));

    // Reverse Z. Have to be done on CPU side because the matrix multiplications
    // add up and it loses precision by the time it's sent to GPU.
    mtxProjection[10] = -(mtxProjection[10] + 1.0f);
    mtxProjection[14] = -mtxProjection[14];
}

// Normally, we could delay setting IsMadeOne, but the game relies on that flag
// being present to handle load priority. To work around that, we can prevent
// IsMadeAll from being set until the compilation is finished. Time for a custom flag!
enum
{
    eDatabaseDataFlags_CompilingPipelines = 0x80
};

// This is passed to pipeline compilation threads to keep the loading screen busy until 
// all of them are finished. A shared pointer makes sure the destructor is called only once.
//struct PipelineTaskToken
//{
//    PipelineTaskType type{};
//    boost::shared_ptr<Hedgehog::Database::CDatabaseData> databaseData;
//
//    PipelineTaskToken() : databaseData()
//    {
//    }
//
//    PipelineTaskToken(const PipelineTaskToken&) = delete;
//
//    PipelineTaskToken(PipelineTaskToken&& other)
//        : type(std::exchange(other.type, PipelineTaskType::Null))
//        , databaseData(std::exchange(other.databaseData, nullptr))
//    {
//    }
//
//    ~PipelineTaskToken()
//    {
//        if (type != PipelineTaskType::Null)
//        {
//            if (databaseData.get() != nullptr)
//                databaseData->m_Flags &= ~eDatabaseDataFlags_CompilingPipelines;
//
//            if ((--g_compilingPipelineTaskCount) == 0)
//                g_compilingPipelineTaskCount.notify_one();
//        }
//    }
//};

//struct PipelineStateQueueItem
//{
//    XXH64_hash_t pipelineHash;
//    PipelineState pipelineState;
//    std::shared_ptr<PipelineTaskToken> token;
//#ifdef ASYNC_PSO_DEBUG
//    std::string pipelineName;
//#endif
//};

//static moodycamel::BlockingConcurrentQueue<PipelineStateQueueItem> g_pipelineStateQueue;

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
    RenderCommand cmd;
    cmd.type = RenderCommandType::AddPipeline;
    cmd.addPipeline.hash = pipelineHash;
    cmd.addPipeline.pipeline = pipeline.release();
    g_renderQueue.enqueue(cmd);
}

static void PipelineCompilerThread()
{
#ifdef _WIN32
    int threadPriority = THREAD_PRIORITY_LOWEST;
    SetThreadPriority(GetCurrentThread(), threadPriority);
    GuestThread::SetThreadName(GetCurrentThreadId(), "Pipeline Compiler Thread");
#endif

    std::unique_ptr<GuestThreadContext> ctx;

//    while (true)
//    {
//        PipelineStateQueueItem queueItem;
//        g_pipelineStateQueue.wait_dequeue(queueItem);
//
//        if (ctx == nullptr)
//            ctx = std::make_unique<GuestThreadContext>(0);
//
//#ifdef _WIN32
//        int newThreadPriority = threadPriority;
//
//        bool loading = *SWA::SGlobals::ms_IsLoading;
//        if (loading)
//            newThreadPriority = THREAD_PRIORITY_HIGHEST;
//        else
//            newThreadPriority = THREAD_PRIORITY_LOWEST;
//
//        if (newThreadPriority != threadPriority)
//        {
//            SetThreadPriority(GetCurrentThread(), newThreadPriority);
//            threadPriority = newThreadPriority;
//        }
//#endif
//
//        CompilePipeline(queueItem.pipelineHash, queueItem.pipelineState
//#ifdef ASYNC_PSO_DEBUG
//            , queueItem.pipelineName.c_str()
//#endif
//        );
//
//        std::this_thread::yield();
//    }
}

static std::vector<std::unique_ptr<std::thread>> g_pipelineCompilerThreads = []()
    {
        size_t threadCount = std::max(2u, (std::thread::hardware_concurrency() * 2) / 3);

        std::vector<std::unique_ptr<std::thread>> threads(threadCount);
        for (auto& thread : threads)
            thread = std::make_unique<std::thread>(PipelineCompilerThread);

        return threads;
    }();

static constexpr uint32_t MODEL_DATA_VFTABLE = 0x82073A44;
static constexpr uint32_t TERRAIN_MODEL_DATA_VFTABLE = 0x8211D25C;
static constexpr uint32_t PARTICLE_MATERIAL_VFTABLE = 0x8211F198;

// Allocate the shared pointer only when new compilations are happening.
// If nothing was compiled, the local "token" variable will get destructed with RAII instead.
struct PipelineTaskTokenPair
{
//    PipelineTaskToken token;
//    std::shared_ptr<PipelineTaskToken> sharedToken;
};

// Having this separate, because I don't want to lock a mutex in the render thread before
// every single draw. Might be worth profiling to see if it actually has an impact and merge them.
static xxHashMap<PipelineState> g_asyncPipelineStates;

//static void EnqueueGraphicsPipelineCompilation(
//    const PipelineState& pipelineState,
//    PipelineTaskTokenPair& tokenPair,
//    const char* name,
//    bool isPrecompiledPipeline = false)
//{
//    XXH64_hash_t hash = XXH3_64bits(&pipelineState, sizeof(pipelineState));
//    bool shouldCompile = g_asyncPipelineStates.emplace(hash, pipelineState).second;
//
//    if (shouldCompile)
//    {
//        bool loading = *SWA::SGlobals::ms_IsLoading;
//        if (!loading && isPrecompiledPipeline)
//        {
//            // We can just compile here during the logos.
//            CompilePipeline(hash, pipelineState
//#ifdef ASYNC_PSO_DEBUG
//                , fmt::format("CACHE {} {:X}", name, hash)
//#endif
//            );
//        }
//        else
//        {
//            if (tokenPair.sharedToken == nullptr && tokenPair.token.type != PipelineTaskType::Null)
//                tokenPair.sharedToken = std::make_shared<PipelineTaskToken>(std::move(tokenPair.token));
//
//            PipelineStateQueueItem queueItem;
//            queueItem.pipelineHash = hash;
//            queueItem.pipelineState = pipelineState;
//            queueItem.token = tokenPair.sharedToken;
//#ifdef ASYNC_PSO_DEBUG
//            queueItem.pipelineName = fmt::format("ASYNC {} {:X}", name, hash);
//#endif
//            g_pipelineStateQueue.enqueue(queueItem);
//        }
//    }
//
//#ifdef PSO_CACHING_CLEANUP
//    if (shouldCompile && isPrecompiledPipeline)
//    {
//        std::lock_guard lock(g_pipelineCacheMutex);
//        g_pipelineStatesToCache.emplace(hash, pipelineState);
//    }
//#endif
//
//#ifdef PSO_CACHING
//    if (!isPrecompiledPipeline)
//    {
//        std::lock_guard lock(g_pipelineCacheMutex);
//        g_pipelineStatesToCache.erase(hash);
//    }
//#endif
//}

struct CompilationArgs
{
    PipelineTaskTokenPair tokenPair;
    bool noGI{};
    bool hasMoreThanOneBone{};
    bool velocityMapQuickStep{};
    bool objectIcon{};
};

enum class MeshLayer
{
    Opaque,
    Transparent,
    PunchThrough,
    Special
};

struct Mesh
{
    uint32_t vertexSize{};
    uint32_t morphTargetVertexSize{};
    GuestVertexDeclaration* vertexDeclaration{};
//    Hedgehog::Mirage::CMaterialData* material{};
    MeshLayer layer{};
    bool morphModel{};
};

//static void CompileMeshPipeline(const Mesh& mesh, CompilationArgs& args)
//{
//    if (mesh.material == nullptr || mesh.material->m_spShaderListData.get() == nullptr)
//        return;
//
//    auto& shaderList = mesh.material->m_spShaderListData;
//
//    bool isFur = !mesh.morphModel && !args.instancing &&
//        strstr(shaderList->m_TypeAndName.c_str(), "Fur") != nullptr;
//
//    bool isSky = !mesh.morphModel && !args.instancing &&
//        strstr(shaderList->m_TypeAndName.c_str(), "Sky") != nullptr;
//
//    bool isSonicMouth = !mesh.morphModel && !args.instancing &&
//        strcmp(mesh.material->m_TypeAndName.c_str() + 2, "sonic_gm_mouth_duble") == 0 &&
//        strcmp(shaderList->m_TypeAndName.c_str() + 3, "SonicSkin_dspf[b]") == 0;
//
//    bool compiledOutsideMainFramebuffer = !args.instancing && !isFur && !isSky;
//
//    bool constTexCoord;
//    if (args.instancing)
//    {
//        constTexCoord = false;
//    }
//    else
//    {
//        constTexCoord = true;
//        if (mesh.material->m_spTexsetData.get() != nullptr)
//        {
//            for (size_t i = 1; i < mesh.material->m_spTexsetData->m_TextureList.size(); i++)
//            {
//                if (mesh.material->m_spTexsetData->m_TextureList[i]->m_TexcoordIndex !=
//                    mesh.material->m_spTexsetData->m_TextureList[0]->m_TexcoordIndex)
//                {
//                    constTexCoord = false;
//                    break;
//                }
//            }
//        }
//    }
//
//    // Shadow pipeline.
//    // if (compiledOutsideMainFramebuffer && (mesh.layer == MeshLayer::Opaque || mesh.layer == MeshLayer::PunchThrough))
//    // {
//    //     PipelineState pipelineState{};
//
//    //     if (mesh.layer == MeshLayer::PunchThrough)
//    //     {
//    //         pipelineState.vertexShader = FindShaderCacheEntry(0xDD4FA7BB53876300)->guestShader;
//    //         pipelineState.pixelShader = FindShaderCacheEntry(0xE2ECA594590DDE8B)->guestShader;
//    //     }
//    //     else
//    //     {
//    //         pipelineState.vertexShader = FindShaderCacheEntry(0x8E4BB23465BD909E)->guestShader;
//    //     }
//
//    //     pipelineState.vertexDeclaration = mesh.vertexDeclaration;
//    //     pipelineState.cullMode = mesh.material->m_DoubleSided ? RenderCullMode::NONE : RenderCullMode::BACK;
//    //     pipelineState.zFunc = RenderComparisonFunction::LESS_EQUAL;
//
//    //     if (g_capabilities.dynamicDepthBias)
//    //     {
//    //         // Put common depth bias values for reducing unnecessary calls.
//    //         if (g_backend == Backend::D3D12)
//    //         {
//    //             pipelineState.depthBias = COMMON_DEPTH_BIAS_VALUE;
//    //             pipelineState.slopeScaledDepthBias = COMMON_SLOPE_SCALED_DEPTH_BIAS_VALUE;
//    //         }
//    //     }
//    //     else
//    //     {
//    //         pipelineState.depthBias = (1 << 24) * (*reinterpret_cast<be<float>*>(g_memory.Translate(0x83302760)));
//    //         pipelineState.slopeScaledDepthBias = *reinterpret_cast<be<float>*>(g_memory.Translate(0x83302764));
//    //     }
//
//    //     pipelineState.colorWriteEnable = 0;
//    //     pipelineState.primitiveTopology = RenderPrimitiveTopology::TRIANGLE_STRIP;
//    //     pipelineState.vertexStrides[0] = mesh.vertexSize;
//    //     pipelineState.depthStencilFormat = RenderFormat::D32_FLOAT;
//
//    //     if (mesh.layer == MeshLayer::PunchThrough)
//    //         pipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TEST;
//
//    //     const char* name = (mesh.layer == MeshLayer::PunchThrough ? "MakeShadowMapTransparent" : "MakeShadowMap");
//    //     SanitizePipelineState(pipelineState);
//    //     EnqueueGraphicsPipelineCompilation(pipelineState, args.tokenPair, name);
//
//    //     // Morph models have 4 targets where unused targets default to the first vertex stream.
//    //     if (mesh.morphModel)
//    //     {
//    //         for (size_t i = 0; i < 5; i++)
//    //         {
//    //             for (size_t j = 0; j < 4; j++)
//    //                 pipelineState.vertexStrides[j + 1] = i > j ? mesh.morphTargetVertexSize : mesh.vertexSize;
//
//    //             SanitizePipelineState(pipelineState);
//    //             EnqueueGraphicsPipelineCompilation(pipelineState, args.tokenPair, name);
//    //         }
//    //     }
//    // }
//
//    // // Motion blur pipeline. We could normally do the player here only, but apparently Werehog enemies also have object blur.
//    // // TODO: Do punch through meshes get rendered?
//    // if (!mesh.morphModel && compiledOutsideMainFramebuffer && args.hasMoreThanOneBone && mesh.layer == MeshLayer::Opaque)
//    // {
//    //     PipelineState pipelineState{};
//    //     pipelineState.vertexShader = FindShaderCacheEntry(0x4620B236DC38100C)->guestShader;
//    //     pipelineState.pixelShader = FindShaderCacheEntry(0xBBDB735BEACC8F41)->guestShader;
//    //     pipelineState.vertexDeclaration = mesh.vertexDeclaration;
//    //     pipelineState.cullMode = RenderCullMode::NONE;
//    //     pipelineState.zFunc = RenderComparisonFunction::GREATER_EQUAL;
//    //     pipelineState.primitiveTopology = RenderPrimitiveTopology::TRIANGLE_STRIP;
//    //     pipelineState.vertexStrides[0] = mesh.vertexSize;
//    //     pipelineState.renderTargetFormat = RenderFormat::R8G8B8A8_UNORM;
//    //     pipelineState.depthStencilFormat = RenderFormat::D32_FLOAT;
//    //     pipelineState.specConstants = SPEC_CONSTANT_REVERSE_Z;
//
//    //     SanitizePipelineState(pipelineState);
//    //     EnqueueGraphicsPipelineCompilation(pipelineState, args.tokenPair, "FxVelocityMap");
//
//    //     if (args.velocityMapQuickStep)
//    //     {
//    //         pipelineState.vertexShader = FindShaderCacheEntry(0x99DC3F27E402700D)->guestShader;
//    //         SanitizePipelineState(pipelineState);
//    //         EnqueueGraphicsPipelineCompilation(pipelineState, args.tokenPair, "FxVelocityMapQuickStep");
//    //     }
//    // }
//
//    uint32_t defaultStr = args.instancing ? 0x820C8734 : 0x8202DDBC; // "instancing" for instancing, "default" for regular
//    guest_stack_var<Hedgehog::Base::CStringSymbol> defaultSymbol(reinterpret_cast<const char*>(g_memory.Translate(defaultStr)));
//    auto defaultFindResult = shaderList->m_PixelShaderPermutations.find(*defaultSymbol);
//    if (defaultFindResult == shaderList->m_PixelShaderPermutations.end())
//        return;
//
//    uint32_t pixelShaderSubPermutationsToCompile = 0;
//    if (constTexCoord) pixelShaderSubPermutationsToCompile |= 0x1;
//    if (args.noGI) pixelShaderSubPermutationsToCompile |= 0x2;
//
//    if ((defaultFindResult->second.m_SubPermutations.get() & (1 << pixelShaderSubPermutationsToCompile)) == 0) pixelShaderSubPermutationsToCompile &= ~0x1;
//    if ((defaultFindResult->second.m_SubPermutations.get() & (1 << pixelShaderSubPermutationsToCompile)) == 0) pixelShaderSubPermutationsToCompile &= ~0x2;
//
//    uint32_t noneStr = mesh.morphModel ? 0x820D72F0 : 0x8200D938; // "p" for morph, "none" for regular
//    guest_stack_var<Hedgehog::Base::CStringSymbol> noneSymbol(reinterpret_cast<const char*>(g_memory.Translate(noneStr)));
//    auto noneFindResult = defaultFindResult->second.m_VertexShaderPermutations.find(*noneSymbol);
//    if (noneFindResult == defaultFindResult->second.m_VertexShaderPermutations.end())
//        return;
//
//    uint32_t vertexShaderSubPermutationsToCompile = 0;
//    if (constTexCoord) vertexShaderSubPermutationsToCompile |= 0x1;
//
//    if ((noneFindResult->second->m_SubPermutations.get() & (1 << vertexShaderSubPermutationsToCompile)) == 0)
//        vertexShaderSubPermutationsToCompile &= ~0x1;
//
//    auto vertexDeclaration = mesh.vertexDeclaration;
//    bool instancing = args.instancing || isFur;
//
//    if (instancing)
//    {
//        GuestVertexElement vertexElements[64];
//        memcpy(vertexElements, mesh.vertexDeclaration->vertexElements.get(), (mesh.vertexDeclaration->vertexElementCount - 1) * sizeof(GuestVertexElement));
//
//        if (args.instancing)
//        {
//            vertexElements[mesh.vertexDeclaration->vertexElementCount - 1] = { 1, 0, 0x2A23B9, 0, 5, 4 };
//            vertexElements[mesh.vertexDeclaration->vertexElementCount] = { 1, 12, 0x2C2159, 0, 5, 5 };
//            vertexElements[mesh.vertexDeclaration->vertexElementCount + 1] = { 1, 16, 0x2C2159, 0, 5, 6 };
//            vertexElements[mesh.vertexDeclaration->vertexElementCount + 2] = { 1, 20, 0x182886, 0, 10, 1 };
//            vertexElements[mesh.vertexDeclaration->vertexElementCount + 3] = { 2, 0, 0x2C82A1, 0, 0, 1 };
//            vertexElements[mesh.vertexDeclaration->vertexElementCount + 4] = D3DDECL_END();
//        }
//        else if (isFur)
//        {
//            vertexElements[mesh.vertexDeclaration->vertexElementCount - 1] = { 1, 0, 0x2C82A1, 0, 0, 1 };
//            vertexElements[mesh.vertexDeclaration->vertexElementCount] = { 2, 0, 0x2C83A4, 0, 0, 2 };
//            vertexElements[mesh.vertexDeclaration->vertexElementCount + 1] = D3DDECL_END();
//        }
//
//        vertexDeclaration = CreateVertexDeclarationWithoutAddRef(vertexElements);
//    }
//
//    for (auto& [pixelShaderSubPermutations, pixelShader] : defaultFindResult->second.m_PixelShaders)
//    {
//        if (pixelShader.get() == nullptr || (pixelShaderSubPermutations & 0x3) != pixelShaderSubPermutationsToCompile)
//            continue;
//
//        for (auto& [vertexShaderSubPermutations, vertexShader] : noneFindResult->second->m_VertexShaders)
//        {
//            if (vertexShader.get() == nullptr || (vertexShaderSubPermutations & 0x1) != vertexShaderSubPermutationsToCompile)
//                continue;
//
//            PipelineState pipelineState{};
//            pipelineState.vertexShader = reinterpret_cast<GuestShader*>(vertexShader->m_spCode->m_pD3DVertexShader.get());
//            pipelineState.pixelShader = reinterpret_cast<GuestShader*>(pixelShader->m_spCode->m_pD3DPixelShader.get());
//            pipelineState.vertexDeclaration = vertexDeclaration;
//            pipelineState.instancing = instancing;
//            pipelineState.zWriteEnable = !isSky && mesh.layer != MeshLayer::Transparent;
//            pipelineState.srcBlend = RenderBlend::SRC_ALPHA;
//            pipelineState.destBlend = mesh.material->m_Additive ? RenderBlend::ONE : RenderBlend::INV_SRC_ALPHA;
//            pipelineState.cullMode = mesh.material->m_DoubleSided ? RenderCullMode::NONE : RenderCullMode::BACK;
//            pipelineState.zFunc = RenderComparisonFunction::GREATER_EQUAL; // Reverse Z
//            pipelineState.alphaBlendEnable = mesh.layer == MeshLayer::Transparent || mesh.layer == MeshLayer::Special;
//            pipelineState.srcBlendAlpha = RenderBlend::SRC_ALPHA;
//            pipelineState.destBlendAlpha = RenderBlend::INV_SRC_ALPHA;
//            pipelineState.primitiveTopology = RenderPrimitiveTopology::TRIANGLE_STRIP;
//            pipelineState.vertexStrides[0] = mesh.vertexSize;
//
//            if (args.instancing)
//            {
//                pipelineState.vertexStrides[1] = 24;
//                pipelineState.vertexStrides[2] = 4;
//            }
//            else if (isFur)
//            {
//                pipelineState.vertexStrides[1] = 4;
//                pipelineState.vertexStrides[2] = 4;
//            }
//
//            pipelineState.renderTargetFormat = RenderFormat::R16G16B16A16_FLOAT;
//            pipelineState.depthStencilFormat = RenderFormat::D32_FLOAT_S8_UINT;
//            pipelineState.sampleCount = Config::AntiAliasing != EAntiAliasing::None ? int32_t(Config::AntiAliasing.Value) : 1;
//
//            if (pipelineState.vertexDeclaration->hasR11G11B10Normal)
//                pipelineState.specConstants |= SPEC_CONSTANT_R11G11B10_NORMAL;
//
//            if (Config::GITextureFiltering == EGITextureFiltering::Bicubic)
//                pipelineState.specConstants |= SPEC_CONSTANT_BICUBIC_GI_FILTER;
//
//            if (mesh.layer == MeshLayer::PunchThrough)
//            {
//                if (Config::AntiAliasing != EAntiAliasing::None && Config::TransparencyAntiAliasing)
//                {
//                    pipelineState.enableAlphaToCoverage = true;
//                    pipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TO_COVERAGE;
//                }
//                else
//                {
//                    pipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TEST;
//                }
//            }
//
//            if (!isSky)
//                pipelineState.specConstants |= SPEC_CONSTANT_REVERSE_Z;
//
//            auto createGraphicsPipeline = [&](PipelineState& pipelineStateToCreate)
//                {
//                    SanitizePipelineState(pipelineStateToCreate);
//                    EnqueueGraphicsPipelineCompilation(pipelineStateToCreate, args.tokenPair, shaderList->m_TypeAndName.c_str() + 3);
//
//                    // Morph models have 4 targets where unused targets default to the first vertex stream.
//                    if (mesh.morphModel)
//                    {
//                        for (size_t i = 0; i < 5; i++)
//                        {
//                            for (size_t j = 0; j < 4; j++)
//                                pipelineStateToCreate.vertexStrides[j + 1] = i > j ? mesh.morphTargetVertexSize : mesh.vertexSize;
//
//                            SanitizePipelineState(pipelineStateToCreate);
//                            EnqueueGraphicsPipelineCompilation(pipelineStateToCreate, args.tokenPair, shaderList->m_TypeAndName.c_str() + 3);
//                        }
//                    }
//                };
//
//            createGraphicsPipeline(pipelineState);
//
//            // We cannot rely on this being accurate during loading as SceneEffect.prm.xml gets loaded a bit later.
//            bool planarReflectionEnabled = *reinterpret_cast<bool*>(g_memory.Translate(0x832FA0D8));
//            bool loading = *SWA::SGlobals::ms_IsLoading;
//            bool compileNoMsaaPipeline = pipelineState.sampleCount != 1 && (loading || planarReflectionEnabled);
//
//            auto noMsaaPipeline = pipelineState;
//            noMsaaPipeline.sampleCount = 1;
//            noMsaaPipeline.enableAlphaToCoverage = false;
//
//            if ((noMsaaPipeline.specConstants & SPEC_CONSTANT_ALPHA_TO_COVERAGE) != 0)
//            {
//                noMsaaPipeline.specConstants &= ~SPEC_CONSTANT_ALPHA_TO_COVERAGE;
//                noMsaaPipeline.specConstants |= SPEC_CONSTANT_ALPHA_TEST;
//            }
//
//            if (compileNoMsaaPipeline)
//            {
//                // Planar reflections don't use MSAA.
//                createGraphicsPipeline(noMsaaPipeline);
//            }
//
//            if (args.objectIcon)
//            {
//                // Object icons get rendered to a SDR buffer without MSAA.
//                auto iconPipelineState = noMsaaPipeline;
//                iconPipelineState.renderTargetFormat = RenderFormat::R8G8B8A8_UNORM;
//                createGraphicsPipeline(iconPipelineState);
//            }
//
//            if (isSonicMouth)
//            {
//                // Sonic's mouth switches between "SonicSkin_dspf[b]" or "SonicSkinNodeInvX_dspf[b]" depending on the view angle.
//                auto mouthPipelineState = pipelineState;
//                mouthPipelineState.vertexShader = FindShaderCacheEntry(0x689AA3140AB9EBAA)->guestShader;
//                createGraphicsPipeline(mouthPipelineState);
//
//                if (compileNoMsaaPipeline)
//                {
//                    auto noMsaaMouthPipelineState = noMsaaPipeline;
//                    noMsaaMouthPipelineState.vertexShader = mouthPipelineState.vertexShader;
//                    createGraphicsPipeline(noMsaaMouthPipelineState);
//                }
//            }
//        }
//    }
//}

//static void CompileMeshPipeline(Hedgehog::Mirage::CMeshData* mesh, MeshLayer layer, CompilationArgs& args)
//{
//    CompileMeshPipeline(Mesh
//        {
//            mesh->m_VertexSize,
//            0,
//            reinterpret_cast<GuestVertexDeclaration*>(mesh->m_VertexDeclarationPtr.m_pD3DVertexDeclaration.get()),
//            mesh->m_spMaterial.get(),
//            layer,
//            false
//        }, args);
//}

//static void CompileMeshPipeline(Hedgehog::Mirage::CMorphModelData* morphModel, Hedgehog::Mirage::CMeshIndexData* mesh, MeshLayer layer, CompilationArgs& args)
//{
//    CompileMeshPipeline(Mesh
//        {
//            morphModel->m_VertexSize,
//            morphModel->m_MorphTargetVertexSize,
//            reinterpret_cast<GuestVertexDeclaration*>(morphModel->m_VertexDeclarationPtr.m_pD3DVertexDeclaration.get()),
//            mesh->m_spMaterial.get(),
//            layer,
//            true
//        }, args);
//}

//template<typename T>
//static void CompileMeshPipelines(const T& modelData, CompilationArgs& args)
//{
//    for (auto& meshGroup : modelData.m_NodeGroupModels)
//    {
//        for (auto& mesh : meshGroup->m_OpaqueMeshes)
//        {
//            CompileMeshPipeline(mesh.get(), MeshLayer::Opaque, args);
//
//            if (args.noGI) // For models that can be shown transparent (eg. medals)
//                CompileMeshPipeline(mesh.get(), MeshLayer::Transparent, args);
//        }
//
//        for (auto& mesh : meshGroup->m_TransparentMeshes)
//            CompileMeshPipeline(mesh.get(), MeshLayer::Transparent, args);
//
//        for (auto& mesh : meshGroup->m_PunchThroughMeshes)
//            CompileMeshPipeline(mesh.get(), MeshLayer::PunchThrough, args);
//
//        for (auto& specialMeshGroup : meshGroup->m_SpecialMeshGroups)
//        {
//            for (auto& mesh : specialMeshGroup)
//                CompileMeshPipeline(mesh.get(), MeshLayer::Special, args); // TODO: Are there layer types other than water in this game??
//        }
//    }
//
//    for (auto& mesh : modelData.m_OpaqueMeshes)
//    {
//        CompileMeshPipeline(mesh.get(), MeshLayer::Opaque, args);
//
//        if (args.noGI)
//            CompileMeshPipeline(mesh.get(), MeshLayer::Transparent, args);
//    }
//
//    for (auto& mesh : modelData.m_TransparentMeshes)
//        CompileMeshPipeline(mesh.get(), MeshLayer::Transparent, args);
//
//    for (auto& mesh : modelData.m_PunchThroughMeshes)
//        CompileMeshPipeline(mesh.get(), MeshLayer::PunchThrough, args);
//
//    if constexpr (std::is_same_v<T, Hedgehog::Mirage::CModelData>)
//    {
//        for (auto& morphModel : modelData.m_MorphModels)
//        {
//            for (auto& mesh : morphModel->m_OpaqueMeshList)
//                CompileMeshPipeline(morphModel.get(), mesh.get(), MeshLayer::Opaque, args);
//
//            for (auto& mesh : morphModel->m_TransparentMeshList)
//                CompileMeshPipeline(morphModel.get(), mesh.get(), MeshLayer::Transparent, args);
//
//            for (auto& mesh : morphModel->m_PunchThroughMeshList)
//                CompileMeshPipeline(morphModel.get(), mesh.get(), MeshLayer::PunchThrough, args);
//        }
//    }
//}

//static void CompileParticleMaterialPipeline(const Hedgehog::Sparkle::CParticleMaterial& material, PipelineTaskTokenPair& tokenPair)
//{
//    auto& shaderList = material.m_spShaderListData;
//    if (shaderList.get() == nullptr)
//        return;
//
//    guest_stack_var<Hedgehog::Base::CStringSymbol> defaultSymbol(reinterpret_cast<const char*>(g_memory.Translate(0x8202DDBC)));
//    auto defaultFindResult = shaderList->m_PixelShaderPermutations.find(*defaultSymbol);
//    if (defaultFindResult == shaderList->m_PixelShaderPermutations.end())
//        return;
//
//    guest_stack_var<Hedgehog::Base::CStringSymbol> noneSymbol(reinterpret_cast<const char*>(g_memory.Translate(0x8200D938)));
//    auto noneFindResult = defaultFindResult->second.m_VertexShaderPermutations.find(*noneSymbol);
//    if (noneFindResult == defaultFindResult->second.m_VertexShaderPermutations.end())
//        return;
//
//    // All the particle models in the game come with the unoptimized format, so we can assume it.
//    uint8_t unoptimizedVertexElements[144] =
//    {
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x23, 0xB9, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x0C, 0x00, 0x2A, 0x23, 0xB9, 0x00, 0x03, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x18, 0x00, 0x2A, 0x23, 0xB9, 0x00, 0x06, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x24, 0x00, 0x2A, 0x23, 0xB9, 0x00, 0x07, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x30, 0x00, 0x2C, 0x23, 0xA5, 0x00, 0x05, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x38, 0x00, 0x2C, 0x23, 0xA5, 0x00, 0x05, 0x01, 0x00,
//        0x00, 0x00, 0x00, 0x40, 0x00, 0x2C, 0x23, 0xA5, 0x00, 0x05, 0x02, 0x00,
//        0x00, 0x00, 0x00, 0x48, 0x00, 0x2C, 0x23, 0xA5, 0x00, 0x05, 0x03, 0x00,
//        0x00, 0x00, 0x00, 0x50, 0x00, 0x1A, 0x23, 0xA6, 0x00, 0x0A, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x60, 0x00, 0x1A, 0x23, 0x86, 0x00, 0x02, 0x00, 0x00,
//        0x00, 0x00, 0x00, 0x64, 0x00, 0x1A, 0x20, 0x86, 0x00, 0x01, 0x00, 0x00,
//        0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00
//    };
//
//    auto unoptimizedVertexDeclaration = CreateVertexDeclarationWithoutAddRef(reinterpret_cast<GuestVertexElement*>(unoptimizedVertexElements));
//    auto sparkleVertexDeclaration = CreateVertexDeclarationWithoutAddRef(reinterpret_cast<GuestVertexElement*>(g_memory.Translate(0x8211F540)));
//
//    bool isMeshShader = strstr(shaderList->m_TypeAndName.c_str(), "Mesh") != nullptr;
//
//    PipelineState pipelineState{};
//    pipelineState.vertexShader = reinterpret_cast<GuestShader*>(noneFindResult->second->m_VertexShaders.begin()->second->m_spCode->m_pD3DVertexShader.get());
//    pipelineState.pixelShader = reinterpret_cast<GuestShader*>(defaultFindResult->second.m_PixelShaders.begin()->second->m_spCode->m_pD3DPixelShader.get());
//    pipelineState.vertexDeclaration = isMeshShader ? unoptimizedVertexDeclaration : sparkleVertexDeclaration;
//    pipelineState.zWriteEnable = false;
//    pipelineState.zFunc = RenderComparisonFunction::GREATER_EQUAL;
//    pipelineState.alphaBlendEnable = true;
//    pipelineState.srcBlendAlpha = RenderBlend::SRC_ALPHA;
//    pipelineState.destBlendAlpha = RenderBlend::INV_SRC_ALPHA;
//    pipelineState.primitiveTopology = RenderPrimitiveTopology::TRIANGLE_STRIP;
//    pipelineState.vertexStrides[0] = isMeshShader ? 104 : 28;
//    pipelineState.depthStencilFormat = RenderFormat::D32_FLOAT_S8_UINT;
//    pipelineState.specConstants = SPEC_CONSTANT_REVERSE_Z;
//
//    if (pipelineState.vertexDeclaration->hasR11G11B10Normal)
//        pipelineState.specConstants |= SPEC_CONSTANT_R11G11B10_NORMAL;
//
//    switch (material.m_BlendMode.get())
//    {
//    case Hedgehog::Sparkle::CParticleMaterial::eBlendMode_Zero:
//        pipelineState.srcBlend = RenderBlend::ZERO;
//        pipelineState.destBlend = RenderBlend::ZERO;
//        break;
//    case Hedgehog::Sparkle::CParticleMaterial::eBlendMode_Typical:
//        pipelineState.srcBlend = RenderBlend::SRC_ALPHA;
//        pipelineState.destBlend = RenderBlend::INV_SRC_ALPHA;
//        break;
//    case Hedgehog::Sparkle::CParticleMaterial::eBlendMode_Add:
//        pipelineState.srcBlend = RenderBlend::SRC_ALPHA;
//        pipelineState.destBlend = RenderBlend::ONE;
//        break;
//    default:
//        pipelineState.srcBlend = RenderBlend::ONE;
//        pipelineState.destBlend = RenderBlend::ONE;
//        break;
//    }
//
//    auto createGraphicsPipeline = [&](PipelineState& pipelineStateToCreate)
//        {
//            SanitizePipelineState(pipelineStateToCreate);
//            EnqueueGraphicsPipelineCompilation(pipelineStateToCreate, tokenPair, shaderList->m_TypeAndName.c_str() + 3);
//        };
//
//    // Mesh particles can use both cull modes. Quad particles are only NONE.
//    RenderCullMode cullModes[] = { RenderCullMode::NONE, RenderCullMode::BACK };
//    uint32_t cullModeCount = isMeshShader ? std::size(cullModes) : 1;
//    RenderFormat renderTargetFormats[] = { RenderFormat::R16G16B16A16_FLOAT, RenderFormat::R8G8B8A8_UNORM };
//
//    for (size_t i = 0; i < cullModeCount; i++)
//    {
//        pipelineState.cullMode = cullModes[i];
//
//        for (auto renderTargetFormat : renderTargetFormats)
//        {
//            pipelineState.renderTargetFormat = renderTargetFormat;
//
//            if (renderTargetFormat == RenderFormat::R16G16B16A16_FLOAT)
//                pipelineState.sampleCount = Config::AntiAliasing != EAntiAliasing::None ? int32_t(Config::AntiAliasing.Value) : 1;
//            else
//                pipelineState.sampleCount = 1;
//
//            createGraphicsPipeline(pipelineState);
//
//            // Always compile no MSAA variant for particles, as the planar
//            // reflection variable isn't reliable at this time of compilation.
//            bool compileNoMsaaPipeline = pipelineState.sampleCount != 1;
//
//            auto noMsaaPipelineState = pipelineState;
//            noMsaaPipelineState.sampleCount = 1;
//
//            if (compileNoMsaaPipeline)
//                createGraphicsPipeline(noMsaaPipelineState);
//
//            if (!isMeshShader)
//            {
//                // Previous compilation was for locus particles. This one will be for quads.
//                auto quadPipelineState = pipelineState;
//                quadPipelineState.primitiveTopology = RenderPrimitiveTopology::TRIANGLE_LIST;
//                createGraphicsPipeline(quadPipelineState);
//
//                if (compileNoMsaaPipeline)
//                {
//                    auto noMsaaQuadPipelineState = noMsaaPipelineState;
//                    noMsaaQuadPipelineState.primitiveTopology = RenderPrimitiveTopology::TRIANGLE_LIST;
//                    createGraphicsPipeline(noMsaaQuadPipelineState);
//                }
//            }
//        }
//    }
//}

static std::thread::id g_mainThreadId = std::this_thread::get_id();

// SWA::CGameModeStage::ExitLoading
// PPC_FUNC_IMPL(__imp__sub_825369A0);
// PPC_FUNC(sub_825369A0)
// {
//     assert(std::this_thread::get_id() == g_mainThreadId);
//
//     // Wait for pipeline compilations to finish.
//     uint32_t value;
//     while ((value = g_compilingPipelineTaskCount.load()) != 0)
//     {
//         // Pump SDL events to prevent the OS
//         // from thinking the process is unresponsive.
//         SDL_PumpEvents();
//         SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
//
//         g_compilingPipelineTaskCount.wait(value);
//     }
//
//     __imp__sub_825369A0(ctx, base);
// }

// CModelData::CheckMadeAll
// PPC_FUNC_IMPL(__imp__sub_82E2EFB0);
// PPC_FUNC(sub_82E2EFB0)
// {   
//     if (reinterpret_cast<Hedgehog::Database::CDatabaseData*>(base + ctx.r3.u32)->m_Flags & eDatabaseDataFlags_CompilingPipelines)
//     {
//         ctx.r3.u64 = 0;
//     }
//     else
//     {
//         __imp__sub_82E2EFB0(ctx, base);
//     }
// }

// CTerrainModelData::CheckMadeAll
// PPC_FUNC_IMPL(__imp__sub_82E243D8);
// PPC_FUNC(sub_82E243D8)
// {   
//     if (reinterpret_cast<Hedgehog::Database::CDatabaseData*>(base + ctx.r3.u32)->m_Flags & eDatabaseDataFlags_CompilingPipelines)
//     {
//         ctx.r3.u64 = 0;
//     }
//     else
//     {
//         __imp__sub_82E243D8(ctx, base);
//     }
// }

// CParticleMaterial::CheckMadeAll
// PPC_FUNC_IMPL(__imp__sub_82E87598);
// PPC_FUNC(sub_82E87598)
// {   
//     if (reinterpret_cast<Hedgehog::Database::CDatabaseData*>(base + ctx.r3.u32)->m_Flags & eDatabaseDataFlags_CompilingPipelines)
//     {
//         ctx.r3.u64 = 0;
//     }
//     else
//     {
//         __imp__sub_82E87598(ctx, base);
//     }
// }

//void GetDatabaseDataMidAsmHook(PPCRegister& r1, PPCRegister& r4)
//{
//    auto& databaseData = *reinterpret_cast<boost::shared_ptr<Hedgehog::Database::CDatabaseData>*>(
//        g_memory.Translate(r1.u32 + 0x58));
//
//    if (!databaseData->IsMadeOne() && r4.u32 != NULL)
//    {
//        if (databaseData->m_pVftable.ptr == MODEL_DATA_VFTABLE)
//        {
//            // Ignore particle models, the materials they point at don't actually
//            // get used and give the threads unnecessary work.
//            bool isParticleModel = *reinterpret_cast<be<uint32_t>*>(g_memory.Translate(r4.u32 + 4)) != 5 &&
//                strncmp(databaseData->m_TypeAndName.c_str() + 2, "eff_", 4) == 0;
//
//            if (isParticleModel)
//                return;
//
//            // Adabat water is broken in original game, which they tried to fix by partially including the files in the update,
//            // which then finally fixed for real in the DLC. This confuses the async PSO compiler and causes a hang if the DLC is missing.
//            // We'll just ignore it.
//            bool isAdabatWater = strcmp(databaseData->m_TypeAndName.c_str() + 2, "evl_sea_obj_st_waterCircle") == 0;
//            if (isAdabatWater)
//                return;
//        }
//
//        databaseData->m_Flags |= eDatabaseDataFlags_CompilingPipelines;
//        EnqueuePipelineTask(PipelineTaskType::DatabaseData, databaseData);
//    }
//}

//static bool CheckMadeAll(Hedgehog::Mirage::CMeshData* meshData)
//{
//    if (!meshData->IsMadeOne())
//        return false;
//
//    if (meshData->m_spMaterial.get() != nullptr)
//    {
//        if (!meshData->m_spMaterial->IsMadeOne())
//            return false;
//
//        if (meshData->m_spMaterial->m_spTexsetData.get() != nullptr)
//        {
//            if (!meshData->m_spMaterial->m_spTexsetData->IsMadeOne())
//                return false;
//
//            for (auto& texture : meshData->m_spMaterial->m_spTexsetData->m_TextureList)
//            {
//                if (!texture->IsMadeOne())
//                    return false;
//            }
//        }
//    }
//
//    return true;
//}

template<typename T>
static bool CheckMadeAll(const T& modelData)
{
    if (!modelData.IsMadeOne())
        return false;

    for (auto& meshGroup : modelData.m_NodeGroupModels)
    {
        for (auto& mesh : meshGroup->m_OpaqueMeshes)
        {
            if (!CheckMadeAll(mesh.get()))
                return false;
        }     

        for (auto& mesh : meshGroup->m_TransparentMeshes)
        {
            if (!CheckMadeAll(mesh.get()))
                return false;
        }    

        for (auto& mesh : meshGroup->m_PunchThroughMeshes)
        {
            if (!CheckMadeAll(mesh.get()))
                return false;
        }

        for (auto& specialMeshGroup : meshGroup->m_SpecialMeshGroups)
        {
            for (auto& mesh : specialMeshGroup)
            {
                if (!CheckMadeAll(mesh.get()))
                    return false;
            }
        }
    }

    for (auto& mesh : modelData.m_OpaqueMeshes)
    {
        if (!CheckMadeAll(mesh.get()))
            return false;
    }

    for (auto& mesh : modelData.m_TransparentMeshes)
    {
        if (!CheckMadeAll(mesh.get()))
            return false;
    }

    for (auto& mesh : modelData.m_PunchThroughMeshes)
    {
        if (!CheckMadeAll(mesh.get()))
            return false;
    }

    return true;
}

//static void PipelineTaskConsumerThread()
//{
//#ifdef _WIN32
//    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
//    GuestThread::SetThreadName(GetCurrentThreadId(), "Pipeline Task Consumer Thread");
//#endif
//
//    std::vector<PipelineTask> localPipelineTaskQueue;
//    std::unique_ptr<GuestThreadContext> ctx;
//
//    while (true)
//    {
//        // Wait for tasks to arrive.
//        uint32_t pendingPipelineTaskCount;
//        while ((pendingPipelineTaskCount = g_pendingPipelineTaskCount.load()) == 0)
//            g_pendingPipelineTaskCount.wait(pendingPipelineTaskCount);
//
//        if (ctx == nullptr)
//            ctx = std::make_unique<GuestThreadContext>(0);
//
//        {
//            std::lock_guard lock(g_pipelineTaskMutex);
//            localPipelineTaskQueue.insert(localPipelineTaskQueue.end(), g_pipelineTaskQueue.begin(), g_pipelineTaskQueue.end());
//            g_pipelineTaskQueue.clear();
//        }
//
//        bool allHandled = true;
//
//        for (auto& [type, databaseData] : localPipelineTaskQueue)
//        {
//            switch (type)
//            {
//            case PipelineTaskType::DatabaseData:
//            {
//                bool ready = false;
//
//                if (databaseData->m_pVftable.ptr == MODEL_DATA_VFTABLE)
//                    ready = CheckMadeAll(*reinterpret_cast<Hedgehog::Mirage::CModelData*>(databaseData.get()));
//                else
//                    ready = databaseData->IsMadeOne();
//
//                if (ready || databaseData.unique())
//                {
//                    if (databaseData->m_pVftable.ptr == TERRAIN_MODEL_DATA_VFTABLE)
//                    {
//                        CompilationArgs args{};
//                        args.tokenPair.token.type = type;
//                        args.tokenPair.token.databaseData = databaseData;
//                        args.instancing = strncmp(databaseData->m_TypeAndName.c_str() + 3, "ins", 3) == 0;
//                        CompileMeshPipelines(*reinterpret_cast<Hedgehog::Mirage::CTerrainModelData*>(databaseData.get()), args);
//                    }
//                    else if (databaseData->m_pVftable.ptr == PARTICLE_MATERIAL_VFTABLE)
//                    {
//                        PipelineTaskTokenPair tokenPair;
//                        tokenPair.token.type = type;
//                        tokenPair.token.databaseData = databaseData;
//                        CompileParticleMaterialPipeline(*reinterpret_cast<Hedgehog::Sparkle::CParticleMaterial*>(databaseData.get()), tokenPair);
//                    }
//                    else
//                    {
//                        assert(databaseData->m_pVftable.ptr == MODEL_DATA_VFTABLE);
//
//                        auto modelData = reinterpret_cast<Hedgehog::Mirage::CModelData*>(databaseData.get());
//
//                        CompilationArgs args{};
//                        args.tokenPair.token.type = type;
//                        args.tokenPair.token.databaseData = databaseData;
//                        args.noGI = true;
//                        args.hasMoreThanOneBone = modelData->m_NodeNum > 1;
//                        args.velocityMapQuickStep = strcmp(databaseData->m_TypeAndName.c_str() + 2, "SonicRoot") == 0;
//
//                        // Check for the on screen items, eg. rings going to HUD.
//                        auto items = reinterpret_cast<xpointer<const char>*>(g_memory.Translate(0x832A8DD0));
//                        for (size_t i = 0; i < 50; i++)
//                        {
//                            if (strcmp(databaseData->m_TypeAndName.c_str() + 2, (*items).get()) == 0)
//                            {
//                                args.objectIcon = true;
//                                break;
//                            }
//                            items += 7;
//                        }
//
//                        CompileMeshPipelines(*modelData, args);
//                    }
//
//                    type = PipelineTaskType::Null;
//                    databaseData = nullptr;
//
//                    --g_pendingPipelineTaskCount;
//                }
//                else
//                {
//                    allHandled = false;
//                }
//
//                break;
//            }
//
//            case PipelineTaskType::PrecompilePipelines:
//            {
//                // Deliberately leaving the type null to account for the enqueue
//                // call not incrementing the compiling pipeline task counter.
//                PipelineTaskTokenPair tokenPair;
//
//                for (auto vertexElements : g_vertexDeclarationCache)
//                    CreateVertexDeclarationWithoutAddRef(reinterpret_cast<GuestVertexElement*>(vertexElements));
//
//                for (auto pipelineState : g_pipelineStateCache)
//                {
//                    // The hashes were reinterpret casted to pointers in the cache.
//                    pipelineState.vertexShader = FindShaderCacheEntry(reinterpret_cast<XXH64_hash_t>(pipelineState.vertexShader))->guestShader;
//
//                    if (pipelineState.pixelShader != nullptr)
//                        pipelineState.pixelShader = FindShaderCacheEntry(reinterpret_cast<XXH64_hash_t>(pipelineState.pixelShader))->guestShader;
//
//                    {
//                        std::lock_guard lock(g_vertexDeclarationMutex);
//                        pipelineState.vertexDeclaration = g_vertexDeclarations[reinterpret_cast<XXH64_hash_t>(pipelineState.vertexDeclaration)];
//                    }
//
//                    if (!g_capabilities.triangleFan && pipelineState.primitiveTopology == RenderPrimitiveTopology::TRIANGLE_FAN)
//                        pipelineState.primitiveTopology = RenderPrimitiveTopology::TRIANGLE_LIST;
//
//                    // Zero out depth bias for Vulkan, we only store common values for D3D12.
//                    if (g_capabilities.dynamicDepthBias && g_backend != Backend::D3D12)
//                    {
//                        pipelineState.depthBias = 0;
//                        pipelineState.slopeScaledDepthBias = 0.0f;
//                    }
//
//                    if (Config::GITextureFiltering == EGITextureFiltering::Bicubic)
//                        pipelineState.specConstants |= SPEC_CONSTANT_BICUBIC_GI_FILTER;
//
//                    auto createGraphicsPipeline = [&](PipelineState& pipelineStateToCreate, const char* name)
//                        {
//                            SanitizePipelineState(pipelineStateToCreate);
//                            EnqueueGraphicsPipelineCompilation(pipelineStateToCreate, tokenPair, name, true);
//                        };
//
//                    // Compile both MSAA and non MSAA variants to work with reflection maps. The render formats are an assumption but it should hold true.
//                    if (Config::AntiAliasing != EAntiAliasing::None &&
//                        pipelineState.renderTargetFormat == RenderFormat::R16G16B16A16_FLOAT &&
//                        pipelineState.depthStencilFormat == RenderFormat::D32_FLOAT_S8_UINT)
//                    {
//                        auto msaaPipelineState = pipelineState;
//                        msaaPipelineState.sampleCount = int32_t(Config::AntiAliasing.Value);
//
//                        if (Config::TransparencyAntiAliasing && (msaaPipelineState.specConstants & SPEC_CONSTANT_ALPHA_TEST) != 0)
//                        {
//                            msaaPipelineState.enableAlphaToCoverage = true;
//                            msaaPipelineState.specConstants &= ~SPEC_CONSTANT_ALPHA_TEST;
//                            msaaPipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TO_COVERAGE;
//                        }
//
//                        createGraphicsPipeline(msaaPipelineState, "Precompiled Pipeline MSAA");
//                    }
//
//                    if (pipelineState.pixelShader != nullptr &&
//                        pipelineState.pixelShader->shaderCacheEntry != nullptr)
//                    {
//                        XXH64_hash_t hash = pipelineState.pixelShader->shaderCacheEntry->hash;
//
//                        // Compile the custom gaussian blur shaders that we pass to the game.
//                        if (hash == 0x4294510C775F4EE8)
//                        {
//                            for (auto& shader : g_gaussianBlurShaders)
//                            {
//                                auto newPipelineState = pipelineState;
//                                newPipelineState.pixelShader = shader.get();
//                                createGraphicsPipeline(newPipelineState, "Precompiled Gaussian Blur Pipeline");
//                            }
//                        }
//                        // Compile enhanced motion blur shader.
//                        else if (hash == 0x6B9732B4CD7E7740)
//                        {
//                            auto newPipelineState = pipelineState;
//                            newPipelineState.pixelShader = g_enhancedMotionBlurShader.get();
//                            createGraphicsPipeline(newPipelineState, "Precompiled Enhanced Motion Blur Pipeline");
//                        }
//                    }
//
//                    createGraphicsPipeline(pipelineState, "Precompiled Pipeline");
//
//                    // Compile the CSD filter shader that we pass to the game when point filtering is used.
//                    if (pipelineState.pixelShader == g_csdShader)
//                    {
//                        pipelineState.pixelShader = g_csdFilterShader.get();
//                        createGraphicsPipeline(pipelineState, "Precompiled CSD Filter Pipeline");
//                    }
//                }
//
//                type = PipelineTaskType::Null;
//                --g_pendingPipelineTaskCount;
//
//                break;
//            }
//
//            case PipelineTaskType::RecompilePipelines:
//            {
//                PipelineTaskTokenPair tokenPair;
//                tokenPair.token.type = type;
//
//                auto asyncPipelines = g_asyncPipelineStates.values();
//
//                for (auto& [hash, pipelineState] : asyncPipelines)
//                {
//                    bool alphaTest = (pipelineState.specConstants & (SPEC_CONSTANT_ALPHA_TEST | SPEC_CONSTANT_ALPHA_TO_COVERAGE)) != 0;
//                    bool msaa = pipelineState.sampleCount != 1 || (pipelineState.renderTargetFormat == RenderFormat::R16G16B16A16_FLOAT && pipelineState.depthStencilFormat == RenderFormat::D32_FLOAT_S8_UINT);
//
//                    pipelineState.sampleCount = 1;
//                    pipelineState.enableAlphaToCoverage = false;
//                    pipelineState.specConstants &= ~(SPEC_CONSTANT_BICUBIC_GI_FILTER | SPEC_CONSTANT_ALPHA_TEST | SPEC_CONSTANT_ALPHA_TO_COVERAGE);
//
//                    if (msaa && Config::AntiAliasing != EAntiAliasing::None)
//                    {
//                        pipelineState.sampleCount = int32_t(Config::AntiAliasing.Value);
//
//                        if (alphaTest)
//                        {
//                            if (Config::TransparencyAntiAliasing)
//                            {
//                                pipelineState.enableAlphaToCoverage = true;
//                                pipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TO_COVERAGE;
//                            }
//                            else
//                            {
//                                pipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TEST;
//                            }
//                        }
//                    }
//                    else if (alphaTest)
//                    {
//                        pipelineState.specConstants |= SPEC_CONSTANT_ALPHA_TEST;
//                    }
//
//                    if (Config::GITextureFiltering == EGITextureFiltering::Bicubic)
//                        pipelineState.specConstants |= SPEC_CONSTANT_BICUBIC_GI_FILTER;
//
//                    SanitizePipelineState(pipelineState);
//                    EnqueueGraphicsPipelineCompilation(pipelineState, tokenPair, "Recompiled Pipeline State");
//                }
//
//                type = PipelineTaskType::Null;
//                --g_pendingPipelineTaskCount;
//
//                break;
//            }
//            }
//        }
//
//        if (allHandled)
//            localPipelineTaskQueue.clear();
//
//        std::this_thread::yield();
//    }
//}

//static std::thread g_pipelineTaskConsumerThread(PipelineTaskConsumerThread);

#ifdef ASYNC_PSO_DEBUG

// PPC_FUNC_IMPL(__imp__sub_82E33330);
// PPC_FUNC(sub_82E33330)
// {
//     auto vertexShaderCode = reinterpret_cast<Hedgehog::Mirage::CVertexShaderCodeData*>(g_memory.Translate(ctx.r4.u32));
//     __imp__sub_82E33330(ctx, base);
//     reinterpret_cast<GuestShader*>(vertexShaderCode->m_pD3DVertexShader.get())->name = vertexShaderCode->m_TypeAndName.c_str() + 3;
// }

// PPC_FUNC_IMPL(__imp__sub_82E328D8);
// PPC_FUNC(sub_82E328D8)
// {
//     auto pixelShaderCode = reinterpret_cast<Hedgehog::Mirage::CPixelShaderCodeData*>(g_memory.Translate(ctx.r4.u32));
//     __imp__sub_82E328D8(ctx, base);
//     reinterpret_cast<GuestShader*>(pixelShaderCode->m_pD3DPixelShader.get())->name = pixelShaderCode->m_TypeAndName.c_str() + 2;
// }

#endif

#ifdef PSO_CACHING
class SDLEventListenerForPSOCaching : public SDLEventListener
{
public:
    bool OnSDLEvent(SDL_Event* event) override 
    {
        if (event->type != SDL_QUIT)
            return false;

        std::lock_guard lock(g_pipelineCacheMutex);
        if (g_pipelineStatesToCache.empty())
            return false;

        FILE* f = fopen("send_this_file_to_skyth.txt", "ab");
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
    g_needsResize |=
        config == &Config::AspectRatio ||
        config == &Config::ResolutionScale ||
        config == &Config::AntiAliasing ||
        config == &Config::ShadowResolution;

    if (g_needsResize)
        Video::ComputeViewportDimensions();
        
    // Config options that require pipeline recompilation
    bool shouldRecompile =
        config == &Config::AntiAliasing ||
        config == &Config::TransparencyAntiAliasing;

//    if (shouldRecompile)
//        EnqueuePipelineTask(PipelineTaskType::RecompilePipelines, {});
}

// SWA::CCsdTexListMirage::SetFilter
// PPC_FUNC_IMPL(__imp__sub_825E4300);
// PPC_FUNC(sub_825E4300)
// {
//     g_csdFilterState = ctx.r5.u32 == 0 ? CsdFilterState::On : CsdFilterState::Off;
//     ctx.r5.u32 = 1;
//     __imp__sub_825E4300(ctx, base);
// }

// SWA::CCsdPlatformMirage::EndScene
// PPC_FUNC_IMPL(__imp__sub_825E2F78);
// PPC_FUNC(sub_825E2F78)
// {
//     g_csdFilterState = CsdFilterState::Unknown;
//     __imp__sub_825E2F78(ctx, base);
// }

// Game shares surfaces with identical descriptions. We don't want to share shadow maps,
// so we can set its format to a depth format that still resolves to the same type in recomp,
// but manages to keep the surfaces actually separated in guest code.
void FxShadowMapInitMidAsmHook(PPCRegister& r11)
{
    uint8_t* base = g_memory.base;

    uint32_t surface = PPC_LOAD_U32(PPC_LOAD_U32(PPC_LOAD_U32(r11.u32 + 0x24) + 0x4));
    PPC_STORE_U32(surface + 0x20, D3DFMT_D24FS8);
}

// Re-render objects in the terrain shadow map instead of copying the texture.
static bool g_jumpOverStretchRect;

void FxShadowMapNoTerrainMidAsmHook(PPCRegister& r4, PPCRegister& r30)
{
    // Set the no terrain shadow map as the render target.
    uint8_t* base = g_memory.base;
    r4.u64 = PPC_LOAD_U32(r30.u32 + 0x58);
}

bool FxShadowMapMidAsmHook(PPCRegister& r4, PPCRegister& r5, PPCRegister& r6, PPCRegister& r30)
{
    if (g_jumpOverStretchRect)
    {
        // Reset for the next time shadow maps get rendered.
        g_jumpOverStretchRect = false;

        // Jump over the stretch rect call.
        return false;
    }
    else
    {
        // Mark to jump over the stretch call the next time.
        g_jumpOverStretchRect = true;

        // Jump to the beginning. Set registers accordingly to set the terrain shadow map as the render target.
        uint8_t* base = g_memory.base;
        r6.u64 = 0;
        r5.u64 = 0;
        r4.u64 = PPC_LOAD_U32(r30.u32 + 0x50);

        return true;
    }
}

// There is a bug on AMD where restart indices cause incorrect culling and prevent some triangles from being rendered.
// This seems to happen on both Windows AMD drivers and Mesa. Converting restart indices to degenerate triangles fixes it.
static void ConvertToDegenerateTriangles(uint16_t* indices, uint32_t indexCount, uint16_t*& newIndices, uint32_t& newIndexCount)
{
    newIndices = reinterpret_cast<uint16_t*>(g_userHeap.Alloc(indexCount * sizeof(uint16_t) * 3));
    newIndexCount = 0;

    bool stripStart = true;
    uint32_t stripSize = 0;
    uint16_t lastIndex = 0;

    for (uint32_t i = 0; i < indexCount; i++)
    {
        uint16_t index = indices[i];
        if (index == 0xFFFF)
        {
            if ((stripSize % 2) != 0)
                newIndices[newIndexCount++] = lastIndex;

            stripStart = true;
            stripSize = 0;
        }
        else 
        {
            if (stripStart && newIndexCount != 0)
            {
                newIndices[newIndexCount++] = lastIndex;
                newIndices[newIndexCount++] = index;
            }

            newIndices[newIndexCount++] = index;
            stripStart = false;
            ++stripSize;
            lastIndex = index;
        }
    }
}

struct MeshResource
{
    MARATHON_INSERT_PADDING(0x4);
    be<uint32_t> indexCount;
    be<uint32_t> indices;
};

static std::vector<uint16_t*> g_newIndicesToFree;

// Hedgehog::Mirage::CMeshData::Make
// PPC_FUNC_IMPL(__imp__sub_82E44AF8);
// PPC_FUNC(sub_82E44AF8)
// {
//     uint16_t* newIndicesToFree = nullptr;

//     auto databaseData = reinterpret_cast<Hedgehog::Database::CDatabaseData*>(base + ctx.r3.u32);
//     if (g_triangleStripWorkaround && !databaseData->IsMadeOne())
//     {
//         auto meshResource = reinterpret_cast<MeshResource*>(base + ctx.r4.u32);

//         if (meshResource->indexCount != 0)
//         {
//             uint16_t* newIndices;
//             uint32_t newIndexCount;

//             ConvertToDegenerateTriangles(
//                 reinterpret_cast<uint16_t*>(base + meshResource->indices),
//                 meshResource->indexCount,
//                 newIndices,
//                 newIndexCount);

//             meshResource->indexCount = newIndexCount;
//             meshResource->indices = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(newIndices) - base);

//             if (PPC_LOAD_U32(0x83396E98) != NULL)
//             {
//                 // If index buffers are getting merged, new indices need to survive until the merge happens.
//                 g_newIndicesToFree.push_back(newIndices);
//             }
//             else 
//             {
//                 // Otherwise, we can free it immediately.
//                 newIndicesToFree = newIndices;
//             }
//         }
//     }

//     __imp__sub_82E44AF8(ctx, base);

//     if (newIndicesToFree != nullptr)
//         g_userHeap.Free(newIndicesToFree);
// }

// Hedgehog::Mirage::CShareVertexBuffer::Reset
// PPC_FUNC_IMPL(__imp__sub_82E250D0);
// PPC_FUNC(sub_82E250D0)
// {
//     __imp__sub_82E250D0(ctx, base);

//     for (auto newIndicesToFree : g_newIndicesToFree)
//         g_userHeap.Free(newIndicesToFree);

//     g_newIndicesToFree.clear();
// }

struct LightAndIndexBufferResourceV1
{
    MARATHON_INSERT_PADDING(0x4);
    be<uint32_t> indexCount;
    be<uint32_t> indices;
};

// Hedgehog::Mirage::CLightAndIndexBufferData::MakeV1
// PPC_FUNC_IMPL(__imp__sub_82E3AFC8);
// PPC_FUNC(sub_82E3AFC8)
// {
//     uint16_t* newIndices = nullptr;

//     auto databaseData = reinterpret_cast<Hedgehog::Database::CDatabaseData*>(base + ctx.r3.u32);
//     if (g_triangleStripWorkaround && !databaseData->IsMadeOne())
//     {
//         auto lightAndIndexBufferResource = reinterpret_cast<LightAndIndexBufferResourceV1*>(base + ctx.r4.u32);

//         if (lightAndIndexBufferResource->indexCount != 0)
//         {
//             uint32_t newIndexCount;

//             ConvertToDegenerateTriangles(
//                 reinterpret_cast<uint16_t*>(base + lightAndIndexBufferResource->indices),
//                 lightAndIndexBufferResource->indexCount,
//                 newIndices,
//                 newIndexCount);

//             lightAndIndexBufferResource->indexCount = newIndexCount;
//             lightAndIndexBufferResource->indices = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(newIndices) - base);
//         }
//     }

//     __imp__sub_82E3AFC8(ctx, base);

//     if (newIndices != nullptr)
//         g_userHeap.Free(newIndices);
// }

struct LightAndIndexBufferResourceV5
{
    MARATHON_INSERT_PADDING(0x8);
    be<uint32_t> indexCount;
    be<uint32_t> indices;
};

// Hedgehog::Mirage::CLightAndIndexBufferData::MakeV5
// PPC_FUNC_IMPL(__imp__sub_82E3B1C0);
// PPC_FUNC(sub_82E3B1C0)
// {
//     uint16_t* newIndices = nullptr;

//     auto databaseData = reinterpret_cast<Hedgehog::Database::CDatabaseData*>(base + ctx.r3.u32);
//     if (g_triangleStripWorkaround && !databaseData->IsMadeOne())
//     {
//         auto lightAndIndexBufferResource = reinterpret_cast<LightAndIndexBufferResourceV5*>(base + ctx.r4.u32);

//         if (lightAndIndexBufferResource->indexCount != 0)
//         {
//             uint32_t newIndexCount;

//             ConvertToDegenerateTriangles(
//                 reinterpret_cast<uint16_t*>(base + lightAndIndexBufferResource->indices),
//                 lightAndIndexBufferResource->indexCount,
//                 newIndices,
//                 newIndexCount);

//             lightAndIndexBufferResource->indexCount = newIndexCount;
//             lightAndIndexBufferResource->indices = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(newIndices) - base);
//         }
//     }

//     __imp__sub_82E3B1C0(ctx, base);

//     if (newIndices != nullptr)
//         g_userHeap.Free(newIndices);
// }

GUEST_FUNCTION_HOOK(sub_8253EC98, CreateDevice); // replaced

GUEST_FUNCTION_HOOK(sub_8253AE98, DestructResource); // replaced

GUEST_FUNCTION_HOOK(sub_8253A740, LockTextureRect); // replaced
GUEST_FUNCTION_HOOK(sub_82538D30, UnlockTextureRect); // GUEST_FUNCTION_HOOK(sub_82BE7780, UnlockTextureRect);

GUEST_FUNCTION_HOOK(sub_8253B5D0, LockVertexBuffer); // replaced
GUEST_FUNCTION_HOOK(sub_8253B630, UnlockVertexBuffer); // replaced
// // GUEST_FUNCTION_HOOK(sub_82BE61D0, GetVertexBufferDesc);

GUEST_FUNCTION_HOOK(sub_8253B6F0, LockIndexBuffer); // replaced
GUEST_FUNCTION_HOOK(sub_8253B750, UnlockIndexBuffer); // replaced
// // GUEST_FUNCTION_HOOK(sub_82BE6200, GetIndexBufferDesc);

GUEST_FUNCTION_HOOK(sub_8253AB20, GetSurfaceDesc); // replaced

GUEST_FUNCTION_HOOK(sub_825471F8, GetVertexDeclaration); // replaced
// // GUEST_FUNCTION_HOOK(sub_82BE0530, HashVertexDeclaration);

GUEST_FUNCTION_HOOK(sub_825586B0, Video::Present);
// GUEST_FUNCTION_HOOK(sub_82558CD0, Video::Present); // wmv player present
GUEST_FUNCTION_HOOK(sub_82543B58, GetBackBuffer);
GUEST_FUNCTION_HOOK(sub_82543BA0, GetDepthStencil);

GUEST_FUNCTION_HOOK(sub_8253A8D8, CreateTexture); // replaced
GUEST_FUNCTION_HOOK(sub_8253B508, CreateVertexBuffer); // replaced
GUEST_FUNCTION_HOOK(sub_8253B640, CreateIndexBuffer); // replaced
GUEST_FUNCTION_HOOK(sub_8253A9F8, CreateSurface); // replaced

GUEST_FUNCTION_HOOK(sub_825575B8, StretchRect); // replaced

GUEST_FUNCTION_HOOK(sub_82543EE0, SetRenderTarget); // replaced
GUEST_FUNCTION_HOOK(sub_825444F0, SetRenderTarget); // replaced
GUEST_FUNCTION_HOOK(sub_82544210, SetDepthStencilSurface); // replaced

GUEST_FUNCTION_HOOK(sub_82555B30, Clear);

GUEST_FUNCTION_HOOK(sub_825436F0, SetViewport); // replaced

GUEST_FUNCTION_HOOK(sub_8253AC40, SetTexture); // replaced
GUEST_FUNCTION_HOOK(sub_82543628, SetScissorRect); // replaced

GUEST_FUNCTION_HOOK(sub_826FEC28, DrawPrimitive); // replaced
GUEST_FUNCTION_HOOK(sub_826FF030, DrawIndexedPrimitive); // replaced
GUEST_FUNCTION_HOOK(sub_826FE5C0, DrawPrimitiveUP);

GUEST_FUNCTION_HOOK(sub_82547118, CreateVertexDeclaration); // replaced
GUEST_FUNCTION_HOOK(sub_825470F8, SetVertexDeclaration); // replaced

GUEST_FUNCTION_HOOK(sub_82548700, CreateVertexShader); // replaced
GUEST_FUNCTION_HOOK(sub_82546EE0, SetVertexShader); // replaced

GUEST_FUNCTION_HOOK(sub_82543918, SetStreamSource); // replaced
GUEST_FUNCTION_HOOK(sub_82543AC8, SetIndices); // replaced

GUEST_FUNCTION_HOOK(sub_82548608, CreatePixelShader); // replaced
GUEST_FUNCTION_HOOK(sub_82546BD8, SetPixelShader);

GUEST_FUNCTION_HOOK(sub_8253B760, IsSet);

GUEST_FUNCTION_HOOK(sub_82543CF0, SetClipPlane); // replaced

GUEST_FUNCTION_HOOK(sub_82541A78, SetRenderState<D3DRS_ZENABLE>);
GUEST_FUNCTION_HOOK(sub_82541AC0, SetRenderState<D3DRS_ZWRITEENABLE>);
GUEST_FUNCTION_HOOK(sub_82541460, SetRenderState<D3DRS_ALPHATESTENABLE>);
GUEST_FUNCTION_HOOK(sub_825415C0, SetRenderState<D3DRS_SRCBLEND>);
GUEST_FUNCTION_HOOK(sub_82541650, SetRenderState<D3DRS_DESTBLEND>);
GUEST_FUNCTION_HOOK(sub_82541400, SetRenderState<D3DRS_CULLMODE>);
GUEST_FUNCTION_HOOK(sub_82541AF0, SetRenderState<D3DRS_ZFUNC>);
GUEST_FUNCTION_HOOK(sub_825418C8, SetRenderState<D3DRS_ALPHAREF>);
GUEST_FUNCTION_HOOK(sub_825414A0, SetRenderState<D3DRS_ALPHABLENDENABLE>);
GUEST_FUNCTION_HOOK(sub_82541530, SetRenderState<D3DRS_BLENDOP>);
GUEST_FUNCTION_HOOK(sub_82543ED0, SetRenderState<D3DRS_SCISSORTESTENABLE>);
GUEST_FUNCTION_HOOK(sub_82541E90, SetRenderState<D3DRS_SLOPESCALEDEPTHBIAS>);
GUEST_FUNCTION_HOOK(sub_82541F58, SetRenderState<D3DRS_DEPTHBIAS>);
GUEST_FUNCTION_HOOK(sub_82541750, SetRenderState<D3DRS_SRCBLENDALPHA>);
GUEST_FUNCTION_HOOK(sub_825417C0, SetRenderState<D3DRS_DESTBLENDALPHA>);
GUEST_FUNCTION_HOOK(sub_825416E0, SetRenderState<D3DRS_BLENDOPALPHA>);
GUEST_FUNCTION_HOOK(sub_82542050, SetRenderState<D3DRS_COLORWRITEENABLE>);
GUEST_FUNCTION_HOOK(sub_82541B30, SetRenderState<D3DRS_STENCILENABLE>);
GUEST_FUNCTION_HOOK(sub_82541B78, SetRenderState<D3DRS_TWOSIDEDSTENCILMODE>);
GUEST_FUNCTION_HOOK(sub_82541BE8, SetRenderState<D3DRS_STENCILFAIL>);
GUEST_FUNCTION_HOOK(sub_82541C28, SetRenderState<D3DRS_STENCILZFAIL>);
GUEST_FUNCTION_HOOK(sub_82541C68, SetRenderState<D3DRS_STENCILPASS>);
GUEST_FUNCTION_HOOK(sub_82541BB8, SetRenderState<D3DRS_STENCILFUNC>);
GUEST_FUNCTION_HOOK(sub_82541D78, SetRenderState<D3DRS_STENCILREF>);
GUEST_FUNCTION_HOOK(sub_82541D98, SetRenderState<D3DRS_STENCILMASK>);
GUEST_FUNCTION_HOOK(sub_82541DB8, SetRenderState<D3DRS_STENCILWRITEMASK>);
GUEST_FUNCTION_HOOK(sub_82541CC8, SetRenderState<D3DRS_CCW_STENCILFAIL>);
GUEST_FUNCTION_HOOK(sub_82541D08, SetRenderState<D3DRS_CCW_STENCILZFAIL>);
GUEST_FUNCTION_HOOK(sub_82541D48, SetRenderState<D3DRS_CCW_STENCILPASS>);
GUEST_FUNCTION_HOOK(sub_82541C98, SetRenderState<D3DRS_CCW_STENCILFUNC>);
GUEST_FUNCTION_HOOK(sub_82541E38, SetRenderState<D3DRS_CLIPPLANEENABLE>);

int GetType(GuestResource* resource) {
    if (resource->type == ResourceType::Texture) return 3;
    if (resource->type == ResourceType::VolumeTexture) return 17;
    if (resource->type == ResourceType::ArrayTexture) return 19;
    LOGF_WARNING("unknown resource type {:d}!", (int32_t)resource->type);
    __builtin_trap();
    return 0;
}

GUEST_FUNCTION_HOOK(sub_8253AE08, GetType);

// Game asks about the size of surface to check if it's need to be tiled, because EDRAM has only 10MB, if size is more than 1024 (kb?), then enable tiling, so we return 0 to disable tiling
int SurfaceSize(uint32_t width, uint32_t height, uint32_t format, uint32_t multisampleLevel) {
    return 0;
}

GUEST_FUNCTION_HOOK(sub_82538D60, SurfaceSize);

// // GUEST_FUNCTION_HOOK(sub_82C003B8, D3DXFillTexture);
// // GUEST_FUNCTION_HOOK(sub_82C00910, D3DXFillVolumeTexture);

GUEST_FUNCTION_HOOK(sub_82656B68, MakePictureData); // GUEST_FUNCTION_HOOK(sub_82E43FC8, MakePictureData);
GUEST_FUNCTION_HOOK(sub_82656DB8, MakePictureData); // GUEST_FUNCTION_HOOK(sub_82E43FC8, MakePictureData);

// // GUEST_FUNCTION_HOOK(sub_82E9EE38, SetResolution);

GUEST_FUNCTION_HOOK(sub_82736178, ScreenShaderInit); // sub_82AE2BF8

// // This is a buggy function that recreates framebuffers
// // if the inverse capture ratio is not 2.0, but the parameter
// // is completely unused and not stored, so it ends up
// // recreating framebuffers every single frame instead.
// // GUEST_FUNCTION_STUB(sub_82BAAD38);

// // GUEST_FUNCTION_STUB(sub_822C15D8);
// // GUEST_FUNCTION_STUB(sub_822C1810);
GUEST_FUNCTION_STUB(sub_8253EB38); // GUEST_FUNCTION_STUB(sub_82BD97A8); // set thread to d3ddevvice
GUEST_FUNCTION_STUB(sub_8253EB78); // GUEST_FUNCTION_STUB(sub_82BD97E8); // reset thread to d3ddevice
// // GUEST_FUNCTION_STUB(sub_82BDD370); // SetGammaRamp
GUEST_FUNCTION_STUB(sub_82543BE0); // SetGammaRamp
GUEST_FUNCTION_STUB(sub_82543C68); // SetGammaRamp
GUEST_FUNCTION_STUB(sub_82547278); // set shader allocation // GUEST_FUNCTION_STUB(sub_82BE05B8);
// GUEST_FUNCTION_STUB(sub_82558F88); // GUEST_FUNCTION_STUB(sub_82BE9C98); // D3DDevice_BeginTiling
// GUEST_FUNCTION_STUB(sub_82559480); // GUEST_FUNCTION_STUB(sub_82BEA308); D3DDevice_EndTiling
GUEST_FUNCTION_STUB(sub_8272FAD0); // GUEST_FUNCTION_STUB(sub_82CD5D68);
GUEST_FUNCTION_STUB(sub_82558E00); // GUEST_FUNCTION_STUB(sub_82BE9B28);
GUEST_FUNCTION_STUB(sub_82559928); // GUEST_FUNCTION_STUB(sub_82BEA018);
GUEST_FUNCTION_STUB(sub_82559C18); // GUEST_FUNCTION_STUB(sub_82BEA7C0);
GUEST_FUNCTION_STUB(sub_82700C18); // GUEST_FUNCTION_STUB(sub_82BFFF88); // D3DXFilterTexture
GUEST_FUNCTION_STUB(sub_8253EAE0);// GUEST_FUNCTION_STUB(sub_82BD96D0);

// GUEST_FUNCTION_STUB(sub_8253E798); // flush
// GUEST_FUNCTION_STUB(sub_825586B0); // swap

// GUEST_FUNCTION_STUB(sub_8259ABE8);


// GUEST_FUNCTION_STUB(sub_82592B70);

// GUEST_FUNCTION_STUB(sub_826FE5C0);
// GUEST_FUNCTION_STUB(sub_828FF2B8); // audio exeutemain
// GUEST_FUNCTION_STUB(sub_829023B8); // audio exeutemain
// GUEST_FUNCTION_STUB(sub_82731038); // something with textures causes memory error
// GUEST_FUNCTION_STUB(sub_82730C68); // something with textures causes memory error
// GUEST_FUNCTION_STUB(sub_8255A8A8); // something audio
// GUEST_FUNCTION_STUB(sub_8255A4D0); // something something
// GUEST_FUNCTION_STUB(sub_82743188); // something render
// GUEST_FUNCTION_STUB(sub_82734C50);

// GUEST_FUNCTION_STUB(sub_827344C0) // I don't know
struct Rect {
    be<uint32_t> x1;
    be<uint32_t> y1;
    be<uint32_t> x2;
    be<uint32_t> y2;
};

struct RESOLVE_PARAMS {
    be<uint32_t> format;
    be<uint32_t> unk;
    be<uint32_t> format2;
};

int D3DDevice_BeginTiling(GuestDevice* device, uint32_t flags, uint32_t count, Rect* pTileRects, be<float>* pClearColor, float clearZ, uint32_t clearStencil) {
    // printf("D3DDevice_BeginTiling: %x %x %x %x %f %x\n", flags, count, pTileRects, pClearColor, clearZ, clearStencil);
    // for (uint32_t i = 0; i < count; i++) {
    //     printf("pTileRects: %d %d %d %d\n", pTileRects[i].x1.get(), pTileRects[i].y1.get(), pTileRects[i].x2.get(), pTileRects[i].y2.get());
    // }
    Clear(device, 0x3F, 0, pClearColor, clearZ, clearStencil);
    return 0;
}

GUEST_FUNCTION_HOOK(sub_82558F88, D3DDevice_BeginTiling);

int D3DDevice_EndTiling(GuestDevice* device, uint32_t flags, Rect* pResolveRects, GuestTexture* pDestTexture, be<float>* pClearColor, float clearZ, uint32_t clearStencil, RESOLVE_PARAMS* resolveParams) {
    // printf("D3DDevice_EndTiling: %x %x %x %x %f %x %x\n", flags, pResolveRects, pDestTexture, pClearColor, clearZ, clearStencil, resolveParams);
    // if (pResolveRects != nullptr) {
    //     printf("pResolveRects: %x %x %x %x\n", pResolveRects->x1.get(), pResolveRects->y1.get(), pResolveRects->x2.get(), pResolveRects->y2.get());
    // }
    // printf("pDestTexture: %x\n", pDestTexture);
    // if (resolveParams != nullptr) {
    //     printf("pResolveParams: %x %x %x\n", resolveParams->format.get(), resolveParams->unk.get(), resolveParams->format2.get());
    // }
    if (pDestTexture) {
        StretchRect(device, flags, 0, pDestTexture, 0, 0, 0);
    }
    return 0;
}

GUEST_FUNCTION_HOOK(sub_82559480, D3DDevice_EndTiling);

int D3DDevice_BeginShaderConstantF4(GuestDevice* device, uint32_t isPixelShader, uint32_t startRegister, be<uint32_t>* cachedConstantData, be<uint32_t>* writeCombinedConstantData, uint32_t vectorCount) {
    uint32_t* constants;
    be<uint64_t>* dirtyFlags;
    if (isPixelShader) {
        constants = &device->pixelShaderFloatConstants[startRegister * 4];
        dirtyFlags = &device->dirtyFlags[1];
    } else {
        constants = &device->vertexShaderFloatConstants[startRegister * 4];
        dirtyFlags = &device->dirtyFlags[0];
    }

    const uint32_t addr = g_memory.MapVirtual(constants);
    *cachedConstantData = addr;
    *writeCombinedConstantData = addr;

    const uint32_t startBit = startRegister >> 2;
    const uint32_t endBit = (startRegister + vectorCount - 1) >> 2;
    const uint64_t dirtyFlag = ~0ull << startBit >> startBit >> (63 - endBit) << (63 - endBit);
    *dirtyFlags = dirtyFlags->get() | dirtyFlag;

    return 0;
}

GUEST_FUNCTION_HOOK(sub_825466E8, D3DDevice_BeginShaderConstantF4);

GUEST_FUNCTION_STUB(sub_8254D598); // BeginConditional
GUEST_FUNCTION_STUB(sub_8254D7B0); // BeginConditional
GUEST_FUNCTION_STUB(sub_8254D9D0); // BeginConditional
GUEST_FUNCTION_STUB(sub_8254DB90); // BeginConditional
GUEST_FUNCTION_STUB(sub_8254DD40); // SetScreenExtentQueryMode
