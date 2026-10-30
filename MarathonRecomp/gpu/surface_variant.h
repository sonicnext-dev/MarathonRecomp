#pragma once

#include "render_command.h"

// Host render target storage for guest surfaces, identified by EDRAM placement.
struct SurfaceVariant
{
    uint32_t base = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t guestFormat = 0;
    RenderSampleCounts sampleCount = RenderSampleCount::COUNT_1;
    RenderFormat format = RenderFormat::UNKNOWN;
    std::unique_ptr<RenderTexture> textureHolder;
    std::unique_ptr<RenderTextureView> textureView;
    uint32_t descriptorIndex = 0;
    // Barrier state is shared by all views, as they render to the same texture.
    RenderTextureLayout layout = RenderTextureLayout::UNKNOWN;
    // Framebuffers are cached here instead of on the guest surface so they don't
    // outlive the host texture when views are destroyed.
    ankerl::unordered_dense::map<const RenderTexture*, std::unique_ptr<RenderFramebuffer>> framebuffers;
    uint32_t refCount = 0;

    uint32_t deferredClearFlags = 0;
    float deferredClearColor[4]{};
    float deferredClearZ = 1.0f;
    uint32_t deferredClearStencil = 0;
};

extern const char* g_surfaceCreationName;

RenderTextureDesc MakeSurfaceTextureDesc(uint32_t width, uint32_t height, RenderFormat format, RenderSampleCounts sampleCount);
SurfaceVariant* AcquireSurfaceVariant(uint32_t base, uint32_t width, uint32_t height, uint32_t guestFormat, RenderSampleCounts sampleCount, bool& created);
bool ReleaseSurfaceVariant(SurfaceVariant* variant);
void PurgeSurfaceVariantFramebuffers();
void PropagateDeferredClear(GuestSurface* renderTarget, GuestSurface* depthStencil, const RenderCommands::Clear& args);
