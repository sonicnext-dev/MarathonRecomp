#include "video.h"

#include <kernel/function.h>
#include <kernel/heap.h>

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

GUEST_FUNCTION_HOOK(sub_8253B5D0, LockVertexBuffer);
GUEST_FUNCTION_HOOK(sub_8253B6F0, LockIndexBuffer);

GUEST_FUNCTION_HOOK(sub_8253AB20, GetSurfaceDesc);
GUEST_FUNCTION_HOOK(sub_825471F8, GetVertexDeclaration);

GUEST_FUNCTION_HOOK(sub_8253B760, IsSet);

GUEST_FUNCTION_HOOK(sub_8253AE08, GetType);
GUEST_FUNCTION_HOOK(sub_82538D60, SurfaceSize);
