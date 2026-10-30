#include "guest_device.h"
#include "render_command.h"

#include <kernel/function.h>
#include <kernel/memory.h>
#include <user/config.h>

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

void ResetIntermediaryUploadAllocator()
{
    g_intermediaryUploadAllocator.reset();
}

template<GuestRenderState TType>
static void SetRenderState(GuestDevice* device, uint32_t value)
{
    g_renderQueue.enqueue(RenderCommands::SetRenderState
    {
        .type = TType, .value = value
    });
}

static void SetTexture(GuestDevice* device, uint32_t index, GuestTexture* texture)
{
    if (Config::GetPreferredControllerIcons() == EControllerIcons::PlayStation &&
        texture != nullptr && texture->patchedTexture != nullptr)
    {
        texture = texture->patchedTexture.get();
    }

    g_renderQueue.enqueue(RenderCommands::SetTexture
    {
        .index = index, .texture = texture
    });
}

static void SetScissorRect(GuestDevice* device, GuestRect* rect)
{
    g_renderQueue.enqueue(RenderCommands::SetScissorRect
    {
        .left = rect->left,
        .top = rect->top,
        .right = rect->right,
        .bottom = rect->bottom
    });
}

static void SetVertexDeclaration(GuestDevice* device, GuestVertexDeclaration* vertexDeclaration)
{
    g_renderQueue.enqueue(RenderCommands::SetVertexDeclaration { vertexDeclaration });
    device->vertexDeclaration = g_memory.MapVirtual(vertexDeclaration);
}

static void SetVertexShader(GuestDevice* device, GuestShader* shader)
{
    g_renderQueue.enqueue(RenderCommands::SetVertexShader { shader });
}

static void SetStreamSource(GuestDevice* device, uint32_t index, GuestBuffer* buffer, uint32_t offset, uint32_t stride)
{
    g_renderQueue.enqueue(RenderCommands::SetStreamSource
    {
        .index = index,
        .buffer = buffer,
        .offset = offset,
        .stride = stride
    });
}

static void SetIndices(GuestDevice* device, GuestBuffer* buffer)
{
    g_renderQueue.enqueue(RenderCommands::SetIndices { buffer });
}

static void SetPixelShader(GuestDevice* device, GuestShader* shader)
{
    g_renderQueue.enqueue(RenderCommands::SetPixelShader { shader });
}

static void BeginConditionalSurvey(GuestDevice* device, uint32_t index)
{
    assert(index < CONDITIONAL_SURVEY_MAX && "Invalid conditional survey index.");

    g_renderQueue.enqueue(RenderCommands::SetConditionalSurvey
    {
        .enabled = true, .index = index
    });
}

static void EndConditionalSurvey(GuestDevice* device)
{
    g_renderQueue.enqueue(RenderCommands::SetConditionalSurvey
    {
        .enabled = false, .index = 0
    });
}

static void BeginConditionalRendering(GuestDevice* device, uint32_t index)
{
    assert(index < CONDITIONAL_SURVEY_MAX && "Invalid conditional rendering index.");

    g_renderQueue.enqueue(RenderCommands::SetConditionalRendering
    {
        .enabled = true, .index = index
    });
}

static void EndConditionalRendering(GuestDevice* device)
{
    g_renderQueue.enqueue(RenderCommands::SetConditionalRendering
    {
        .enabled = false, .index = 0
    });
}

static void SetClipPlane(GuestDevice* device, uint32_t index, const be<float>* plane)
{
    if (index != 0)
        return;

    g_renderQueue.enqueue(RenderCommands::SetClipPlane
    {
        .plane = { plane[0].get(), plane[1].get(), plane[2].get(), plane[3].get() }
    });
}

struct LocalRenderCommandQueue
{
    RenderCommand commands[20];
    uint32_t count = 0;

    void enqueue(const RenderCommand& command)
    {
        assert(count < std::size(commands));
        commands[count++] = command;
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
        queue.enqueue(RenderCommands::SetBooleans
        {
            (device->vertexShaderBoolConstants[0].get() & 0xFF) | ((device->pixelShaderBoolConstants[0].get() & 0xFF) << 16)
        });

        device->dirtyFlags[3] = device->dirtyFlags[3].get() & ~BOOL_MASK;
    }

    for (uint32_t i = 0; i < 16; i++)
    {
        const size_t mask = 0x8000000000000000ull >> (i + 20);
        if (device->dirtyFlags[2].get() & mask)
        {
            queue.enqueue(RenderCommands::SetSamplerState
            {
                .index = i,
                .data0 = device->samplerStates[i].data[0],
                .data3 = device->samplerStates[i].data[3],
                .data5 = device->samplerStates[i].data[5]
            });

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

        queue.enqueue(RenderCommands::SetVertexShaderConstants
        {
            .memory = g_intermediaryUploadAllocator.allocate(&device->vertexShaderFloatConstants[index], size),
            .index = index,
            .size = size
        });

        device->dirtyFlags[0] = 0;
    }

    dirtyFlags = device->dirtyFlags[1].get();
    if (dirtyFlags != 0)
    {
        int startRegister = std::countl_zero(dirtyFlags);
        int endRegister = std::min(56, 64 - std::countr_zero(dirtyFlags));

        uint32_t index = startRegister * 16;
        uint32_t size = (endRegister - startRegister) * 64;

        queue.enqueue(RenderCommands::SetPixelShaderConstants
        {
            .memory = g_intermediaryUploadAllocator.allocate(&device->pixelShaderFloatConstants[index], size),
            .index = index,
            .size = size
        });

        device->dirtyFlags[1] = 0;
    }
}

static void DrawPrimitive(GuestDevice* device, uint32_t primitiveType, uint32_t startVertex, uint32_t primitiveCount)
{
    LocalRenderCommandQueue queue;
    FlushRenderStateForMainThread(device, queue);

    queue.enqueue(RenderCommands::DrawPrimitive
    {
        .primitiveType = primitiveType,
        .startVertex = startVertex,
        .primitiveCount = primitiveCount
    });

    queue.submit();
}

static void DrawIndexedPrimitive(GuestDevice* device, uint32_t primitiveType, int32_t baseVertexIndex, uint32_t startIndex, uint32_t primCount)
{
    LocalRenderCommandQueue queue;
    FlushRenderStateForMainThread(device, queue);

    queue.enqueue(RenderCommands::DrawIndexedPrimitive
    {
        .primitiveType = primitiveType,
        .baseVertexIndex = baseVertexIndex,
        .startIndex = startIndex,
        .primCount = primCount
    });

    queue.submit();
}

static void DrawPrimitiveUP(GuestDevice* device, uint32_t primitiveType, uint32_t primitiveCount, void* vertexStreamZeroData, uint32_t vertexStreamZeroStride)
{
    LocalRenderCommandQueue queue;
    FlushRenderStateForMainThread(device, queue);

    queue.enqueue(RenderCommands::DrawPrimitiveUP
    {
        .primitiveType = primitiveType,
        .primitiveCount = primitiveCount,
        .vertexStreamZeroData = g_intermediaryUploadAllocator.allocate(vertexStreamZeroData, primitiveCount * vertexStreamZeroStride),
        .vertexStreamZeroSize = primitiveCount * vertexStreamZeroStride,
        .vertexStreamZeroStride = vertexStreamZeroStride
    });

    queue.submit();
}

static void StretchRect(GuestDevice* device, uint32_t flags, uint32_t, GuestTexture* texture, uint32_t, uint32_t, uint32_t destSliceOrFace)
{
    g_renderQueue.enqueue(RenderCommands::StretchRect
    {
        .flags = flags,
        .texture = texture,
        .destSliceOrFace = destSliceOrFace
    });
}

static void SetDefaultViewport(GuestDevice* device, GuestSurface* surface)
{
    if (surface != nullptr)
    {
        g_renderQueue.enqueue(RenderCommands::SetViewport
        {
            .x = 0.0f,
            .y = 0.0f,
            .width = float(surface->width),
            .height = float(surface->height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        });

        device->viewport.x = 0.0f;
        device->viewport.y = 0.0f;
        device->viewport.width = float(surface->width);
        device->viewport.height = float(surface->height);
        device->viewport.minZ = 0.0f;
        device->viewport.maxZ = 1.0f;
    }
}

void SetRenderTarget(GuestDevice* device, uint32_t index, GuestSurface* renderTarget)
{
    if (index == 0)
    {
        g_renderQueue.enqueue(RenderCommands::SetRenderTarget { renderTarget });

        SetDefaultViewport(device, renderTarget);
    }
    else
    {
        // Multiple targets are not currently handled. Make sure any attempt to set them is nullptr.
        assert(renderTarget == nullptr);
    }
}

void SetDepthStencilSurface(GuestDevice* device, GuestSurface* depthStencil)
{
    g_renderQueue.enqueue(RenderCommands::SetDepthStencilSurface { depthStencil });

    SetDefaultViewport(device, depthStencil);
}

static void Clear(GuestDevice* device, uint32_t flags, uint32_t, be<float>* color, double z, uint32_t stencil)
{
    g_renderQueue.enqueue(RenderCommands::Clear
    {
        .flags = flags,
        .color = { color[0], color[1], color[2], color[3] },
        .z = float(z),
        .stencil = stencil
    });
}

static void SetViewport(GuestDevice* device, GuestViewport* viewport)
{
    g_renderQueue.enqueue(RenderCommands::SetViewport
    {
         .x = float(viewport->x),
         .y = float(viewport->y),
         .width = float(viewport->width),
         .height = float(viewport->height),
         .minDepth = viewport->minZ,
         .maxDepth = viewport->maxZ
     });

    device->viewport.x = float(viewport->x);
    device->viewport.y = float(viewport->y);
    device->viewport.width = float(viewport->width);
    device->viewport.height = float(viewport->height);
    device->viewport.minZ = viewport->minZ;
    device->viewport.maxZ = viewport->maxZ;
}

struct Rect
{
    be<uint32_t> x1;
    be<uint32_t> y1;
    be<uint32_t> x2;
    be<uint32_t> y2;
};

struct RESOLVE_PARAMS
{
    be<uint32_t> format;
    be<uint32_t> unk;
    be<uint32_t> format2;
};

int D3DDevice_BeginTiling(GuestDevice* device, uint32_t flags, uint32_t count, Rect* pTileRects, be<float>* pClearColor, float clearZ, uint32_t clearStencil)
{
    Clear(device, 0x3F, 0, pClearColor, clearZ, clearStencil);

    return 0;
}

int D3DDevice_EndTiling(GuestDevice* device, uint32_t flags, Rect* pResolveRects, GuestTexture* pDestTexture, be<float>* pClearColor, float clearZ, uint32_t clearStencil, RESOLVE_PARAMS* resolveParams)
{
    if (pDestTexture)
    {
        StretchRect(device, flags, 0, pDestTexture, 0, 0, 0);
    }

    return 0;
}

int D3DDevice_BeginShaderConstantF4(GuestDevice* device, uint32_t isPixelShader, uint32_t startRegister, be<uint32_t>* cachedConstantData, be<uint32_t>* writeCombinedConstantData, uint32_t vectorCount)
{
    uint32_t* constants;
    be<uint64_t>* dirtyFlags;

    if (isPixelShader)
    {
        constants = &device->pixelShaderFloatConstants[startRegister * 4];
        dirtyFlags = &device->dirtyFlags[1];
    }
    else
    {
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

GUEST_FUNCTION_HOOK(sub_8253AC40, SetTexture);
GUEST_FUNCTION_HOOK(sub_82543628, SetScissorRect);

GUEST_FUNCTION_HOOK(sub_825470F8, SetVertexDeclaration);
GUEST_FUNCTION_HOOK(sub_82546EE0, SetVertexShader);
GUEST_FUNCTION_HOOK(sub_82546BD8, SetPixelShader);

GUEST_FUNCTION_HOOK(sub_82543918, SetStreamSource);
GUEST_FUNCTION_HOOK(sub_82543AC8, SetIndices);

GUEST_FUNCTION_HOOK(sub_82636BF8, BeginConditionalSurvey);
GUEST_FUNCTION_HOOK(sub_82636C08, EndConditionalSurvey);
GUEST_FUNCTION_HOOK(sub_82636C10, BeginConditionalRendering);
GUEST_FUNCTION_HOOK(sub_82636C18, EndConditionalRendering);

GUEST_FUNCTION_HOOK(sub_82543CF0, SetClipPlane);

GUEST_FUNCTION_HOOK(sub_826FEC28, DrawPrimitive);
GUEST_FUNCTION_HOOK(sub_826FF030, DrawIndexedPrimitive);
GUEST_FUNCTION_HOOK(sub_826FE5C0, DrawPrimitiveUP);

GUEST_FUNCTION_HOOK(sub_825575B8, StretchRect);

GUEST_FUNCTION_HOOK(sub_82543EE0, SetRenderTarget);
GUEST_FUNCTION_HOOK(sub_825444F0, SetRenderTarget);
GUEST_FUNCTION_HOOK(sub_82544210, SetDepthStencilSurface);

GUEST_FUNCTION_HOOK(sub_82555B30, Clear);

GUEST_FUNCTION_HOOK(sub_825436F0, SetViewport);

GUEST_FUNCTION_HOOK(sub_82558F88, D3DDevice_BeginTiling);
GUEST_FUNCTION_HOOK(sub_82559480, D3DDevice_EndTiling);
GUEST_FUNCTION_HOOK(sub_825466E8, D3DDevice_BeginShaderConstantF4);
