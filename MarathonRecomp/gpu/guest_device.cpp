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
