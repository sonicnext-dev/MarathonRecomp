#include "resources.h"
#include "render_command.h"
#include "surface_variant.h"
#include "video_internal.h"
#include "guest_convert.h"

#include <kernel/function.h>
#include <kernel/heap.h>
#include <kernel/memory.h>
#include <user/config.h>

std::atomic<uint32_t> g_bufferUploadCount = 0;

uint32_t ComputeTexturePitch(GuestTexture* texture)
{
    return (texture->width * RenderFormatSize(texture->format) + PITCH_ALIGNMENT - 1) & ~(PITCH_ALIGNMENT - 1);
}

GuestTexture* CreateTexture(uint32_t width, uint32_t height, uint32_t depth, uint32_t levels, uint32_t usage, uint32_t format, uint32_t pool, uint32_t type)
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

    texture->textureHolder = Video::GetRenderDevice()->createTexture(desc);
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

    if (desc.flags != RenderTextureFlag::NONE)
    {
        DiscardTexture(texture, desc.flags == RenderTextureFlag::RENDER_TARGET ?
            RenderTextureLayout::COLOR_WRITE : RenderTextureLayout::DEPTH_WRITE);
    }

    return texture;
}

static RenderSampleCounts GetSurfaceSampleCount(uint32_t multiSampleType) {
    if (multiSampleType == 0)
        return RenderSampleCount::COUNT_1;

    switch (Config::AntiAliasing)
    {
    case EAntiAliasing::MSAA2x:
        return RenderSampleCount::COUNT_2;
    case EAntiAliasing::MSAA4x:
        return RenderSampleCount::COUNT_4;
    case EAntiAliasing::MSAA8x:
        return RenderSampleCount::COUNT_8;
    default:
        return RenderSampleCount::COUNT_1;
    }
}

GuestSurface* CreateSurface(uint32_t width, uint32_t height, uint32_t format, uint32_t multiSample, const GuestSurfaceCreateParams* params)
{
    const RenderFormat renderFormat = ConvertFormat(format);
    const RenderSampleCounts sampleCount = GetSurfaceSampleCount(multiSample);
    const bool isDepthStencil = RenderFormatIsDepth(renderFormat);

    const auto surface = g_userHeap.AllocPhysical<GuestSurface>(isDepthStencil ?
        ResourceType::DepthStencil : ResourceType::RenderTarget);

    surface->width = width;
    surface->height = height;
    surface->format = renderFormat;
    surface->guestFormat = format;
    surface->multiSampleType = multiSample;
    surface->sampleCount = sampleCount;

    bool created = false;

    if (params != nullptr)
    {
        // The view shares the variant's host resources without owning them.
        surface->variant = AcquireSurfaceVariant(params->base, width, height, format, sampleCount, created);
        surface->texture = surface->variant->textureHolder.get();
        surface->descriptorIndex = surface->variant->descriptorIndex;
    }
    else
    {
        // No EDRAM placement information: the surface owns its host texture.
        const auto desc = MakeSurfaceTextureDesc(width, height, renderFormat, sampleCount);
        surface->textureHolder = Video::GetRenderDevice()->createTexture(desc);
        surface->texture = surface->textureHolder.get();

        RenderTextureViewDesc viewDesc;
        viewDesc.dimension = RenderTextureViewDimension::TEXTURE_2D;
        viewDesc.format = renderFormat;
        viewDesc.mipLevels = 1;
        surface->textureView = surface->textureHolder->createTextureView(viewDesc);
        surface->descriptorIndex = g_textureDescriptorAllocator.allocate();
        g_textureDescriptorSet->setTexture(surface->descriptorIndex, surface->textureHolder.get(), RenderTextureLayout::SHADER_READ, surface->textureView.get());

    #ifdef _DEBUG
        surface->texture->setName(fmt::format("{} {:X}", isDepthStencil ? "Depth Stencil" : "Render Target", g_memory.MapVirtual(surface)));
    #endif

        created = true;
    }

    if (created)
    {
        DiscardTexture(surface, isDepthStencil ?
            RenderTextureLayout::DEPTH_WRITE : RenderTextureLayout::COLOR_WRITE);
    }

    return surface;
}

void DestructResource(GuestResource* resource)
{
    g_renderQueue.enqueue(RenderCommands::DestructResource { resource });
}

void ReleaseResource(GuestResource* resource)
{
    resource->Release();

    if (resource->refCount == 0)
        DestructResource(resource);
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
    assert(std::this_thread::get_id() == g_mainThreadId);

    g_renderQueue.enqueue(RenderCommands::UnlockTextureRect { texture });
}

static RenderHeapType GetBufferHeapType()
{
    return Video::GetRenderDevice()->getCapabilities().gpuUploadHeap ? RenderHeapType::GPU_UPLOAD : RenderHeapType::DEFAULT;
}

static GuestBuffer* CreateVertexBuffer(uint32_t length)
{
    auto buffer = g_userHeap.AllocPhysical<GuestBuffer>(ResourceType::VertexBuffer);
    buffer->buffer = Video::GetRenderDevice()->createBuffer(RenderBufferDesc::VertexBuffer(length, GetBufferHeapType(), RenderBufferFlag::INDEX));
    buffer->dataSize = length;
#ifdef _DEBUG
    buffer->buffer->setName(fmt::format("Vertex Buffer {:X}", g_memory.MapVirtual(buffer)));
#endif
    return buffer;
}

static GuestBuffer* CreateIndexBuffer(uint32_t length, uint32_t, uint32_t format)
{
    auto buffer = g_userHeap.AllocPhysical<GuestBuffer>(ResourceType::IndexBuffer);
    buffer->buffer = Video::GetRenderDevice()->createBuffer(RenderBufferDesc::IndexBuffer(length, GetBufferHeapType()));
    buffer->dataSize = length;
    buffer->format = ConvertFormat(format);
    buffer->guestFormat = format;
#ifdef _DEBUG
    buffer->buffer->setName(fmt::format("Index Buffer {:X}", g_memory.MapVirtual(buffer)));
#endif
    return buffer;
}

template<typename T>
static void UnlockBuffer(GuestBuffer* buffer)
{
    if (buffer->lockedReadOnly)
        return;

    auto copyBuffer = [&](T* dest)
    {
        auto src = static_cast<const T*>(buffer->mappedMemory);

        for (size_t i = 0; i < buffer->dataSize; i += sizeof(T))
        {
            *dest = ByteSwap(*src);
            ++dest;
            ++src;
        }
    };

    if (Video::GetRenderDevice()->getCapabilities().gpuUploadHeap)
    {
        copyBuffer(static_cast<T*>(buffer->buffer->map()));
        buffer->buffer->unmap();
    }
    else
    {
        auto uploadBuffer = Video::GetRenderDevice()->createBuffer(RenderBufferDesc::UploadBuffer(buffer->dataSize));
        copyBuffer(static_cast<T*>(uploadBuffer->map()));
        uploadBuffer->unmap();

        Video::ExecuteCopyCommandList([&](RenderCommandList* in_pCommandList)
        {
            in_pCommandList->copyBufferRegion(buffer->buffer->at(0), uploadBuffer->at(0), buffer->dataSize);
        });
    }

    ++g_bufferUploadCount;
}

static void UnlockVertexBuffer(GuestBuffer* buffer)
{
    UnlockBuffer<uint32_t>(buffer);
}

static void UnlockIndexBuffer(GuestBuffer* buffer)
{
    if (buffer->guestFormat == D3DFMT_INDEX32)
        UnlockBuffer<uint32_t>(buffer);
    else
        UnlockBuffer<uint16_t>(buffer);
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

static void* LockIndexBuffer(GuestBuffer* buffer, uint32_t, uint32_t, uint32_t flags)
{
    return LockBuffer(buffer, flags);
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
    desc->multiSampleType = surface->multiSampleType;
    desc->multiSampleQuality = 0;
    desc->usage = 0;
}

static void GetVertexDeclaration(const GuestVertexDeclaration* vertexDeclaration, GuestVertexElement* vertexElements, be<uint32_t>* count)
{
    memcpy(vertexElements, vertexDeclaration->vertexElements.get(), vertexDeclaration->vertexElementCount * sizeof(GuestVertexElement));
    *count = vertexDeclaration->vertexElementCount;
}

// Needed for correct clearing of index buffer
static bool IsSet() {
    return true;
}

int GetType(GuestResource* resource)
{
    if (resource->type == ResourceType::Texture) return 3;
    if (resource->type == ResourceType::VolumeTexture) return 17;
    if (resource->type == ResourceType::ArrayTexture) return 19;

    LOGF_WARNING("unknown resource type {:d}!", (int32_t)resource->type);
    __builtin_trap();
    return 0;
}

// Game asks about the size of surface to check if it needs to be tiled.
// Because EDRAM has only 10MB, if size is more than 1024, then it enables tiling.
// We return 0 to always disable tiling.
int SurfaceSize(uint32_t width, uint32_t height, uint32_t format, uint32_t multisampleLevel)
{
    return 0;
}

GUEST_FUNCTION_HOOK(sub_8253AE98, DestructResource);

GUEST_FUNCTION_HOOK(sub_8253A740, LockTextureRect);
GUEST_FUNCTION_HOOK(sub_82538D30, UnlockTextureRect);

GUEST_FUNCTION_HOOK(sub_8253A8D8, CreateTexture);
GUEST_FUNCTION_HOOK(sub_8253A9F8, CreateSurface);

GUEST_FUNCTION_HOOK(sub_8253B5D0, LockVertexBuffer);
GUEST_FUNCTION_HOOK(sub_8253B6F0, LockIndexBuffer);

GUEST_FUNCTION_HOOK(sub_8253B630, UnlockVertexBuffer);
GUEST_FUNCTION_HOOK(sub_8253B750, UnlockIndexBuffer);

GUEST_FUNCTION_HOOK(sub_8253B508, CreateVertexBuffer);
GUEST_FUNCTION_HOOK(sub_8253B640, CreateIndexBuffer);

GUEST_FUNCTION_HOOK(sub_8253AB20, GetSurfaceDesc);
GUEST_FUNCTION_HOOK(sub_825471F8, GetVertexDeclaration);

GUEST_FUNCTION_HOOK(sub_8253B760, IsSet);

GUEST_FUNCTION_HOOK(sub_8253AE08, GetType);
GUEST_FUNCTION_HOOK(sub_82538D60, SurfaceSize);
