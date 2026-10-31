#include "surface_variant.h"
#include "guest_convert.h"

static Mutex g_surfaceVariantMutex;
static std::vector<std::unique_ptr<SurfaceVariant>> g_surfaceVariants;

const char* g_surfaceCreationName;

bool ReleaseSurfaceVariant(SurfaceVariant* variant)
{
    std::lock_guard lock(g_surfaceVariantMutex);

    assert(variant->refCount != 0);
    if (--variant->refCount != 0)
        return false;

    g_textureDescriptorSet->setTexture(variant->descriptorIndex, nullptr, {});
    g_textureDescriptorAllocator.free(variant->descriptorIndex);

    for (auto& other : g_surfaceVariants)
    {
        if (other.get() != variant)
            other->framebuffers.erase(variant->textureHolder.get());
    }

    std::erase_if(g_surfaceVariants, [variant](auto& entry) { return entry.get() == variant; });

    return true;
}

void PurgeSurfaceVariantFramebuffers()
{
    std::lock_guard lock(g_surfaceVariantMutex);

    for (auto& variant : g_surfaceVariants)
        variant->framebuffers.clear();
}

RenderTextureDesc MakeSurfaceTextureDesc(uint32_t width, uint32_t height, RenderFormat format, RenderSampleCounts sampleCount)
{
    RenderTextureDesc desc;
    desc.dimension = RenderTextureDimension::TEXTURE_2D;
    desc.width = width;
    desc.height = height;
    desc.depth = 1;
    desc.mipLevels = 1;
    desc.arraySize = 1;
    desc.multisampling.sampleCount = sampleCount;
    desc.format = format;
    desc.flags = RenderFormatIsDepth(format) ? RenderTextureFlag::DEPTH_TARGET : RenderTextureFlag::RENDER_TARGET;
    return desc;
}

SurfaceVariant* AcquireSurfaceVariant(uint32_t base, uint32_t width, uint32_t height,
    uint32_t guestFormat, RenderSampleCounts sampleCount, bool& created)
{
    std::lock_guard lock(g_surfaceVariantMutex);

    for (auto& variant : g_surfaceVariants)
    {
        if (variant->base == base &&
            variant->width == width &&
            variant->height == height &&
            variant->guestFormat == guestFormat &&
            variant->sampleCount == sampleCount)
        {
            variant->refCount++;
            created = false;

#ifdef _DEBUG
            LOGF_UTILITY("[surface] shared variant: base {} {}x{} format {:X} refs {} name {}",
                base, width, height, guestFormat, variant->refCount,
                g_surfaceCreationName ? g_surfaceCreationName : "<none>");
#endif

            return variant.get();
        }
    }

#ifdef _DEBUG
    LOGF_UTILITY("[surface] new variant: base {} {}x{} format {:X} samples {} name {}",
        base, width, height, guestFormat, uint32_t(sampleCount),
        g_surfaceCreationName ? g_surfaceCreationName : "<none>");
#endif

    auto& variant = g_surfaceVariants.emplace_back(std::make_unique<SurfaceVariant>());
    variant->base = base;
    variant->width = width;
    variant->height = height;
    variant->guestFormat = guestFormat;
    variant->sampleCount = sampleCount;
    variant->format = ConvertFormat(guestFormat);
    variant->refCount = 1;

    const auto desc = MakeSurfaceTextureDesc(width, height, variant->format, sampleCount);
    variant->textureHolder = Video::GetRenderDevice()->createTexture(desc);

    RenderTextureViewDesc viewDesc;
    viewDesc.dimension = RenderTextureViewDimension::TEXTURE_2D;
    viewDesc.format = variant->format;
    viewDesc.mipLevels = 1;
    variant->textureView = variant->textureHolder->createTextureView(viewDesc);
    variant->descriptorIndex = g_textureDescriptorAllocator.allocate();
    g_textureDescriptorSet->setTexture(variant->descriptorIndex, variant->textureHolder.get(), RenderTextureLayout::SHADER_READ, variant->textureView.get());

#ifdef _DEBUG
    variant->textureHolder->setName(fmt::format("{} EDRAM {} {}x{} {}x",
        RenderFormatIsDepth(variant->format) ? "Depth Stencil" : "Render Target",
        base, width, height, int32_t(sampleCount)));
#endif

    created = true;
    return variant.get();
}

void PropagateDeferredClear(GuestSurface* renderTarget, GuestSurface* depthStencil, const RenderCommands::Clear& args)
{
    std::lock_guard lock(g_surfaceVariantMutex);

    for (const auto boundSurface : { renderTarget, depthStencil })
    {
        if (boundSurface == nullptr || boundSurface->variant == nullptr)
            continue;

        const bool boundIsDepth = RenderFormatIsDepth(boundSurface->format);
        const uint32_t flags = args.flags & (boundIsDepth ? (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL) : D3DCLEAR_TARGET);
        if (flags == 0)
            continue;

        // The variant the game just cleared itself needs no deferred clear.
        boundSurface->variant->deferredClearFlags &= ~flags;

        for (auto& variant : g_surfaceVariants)
        {
            if (variant.get() == boundSurface->variant ||
                variant->base != boundSurface->variant->base ||
                RenderFormatIsDepth(variant->format) != boundIsDepth ||
                variant->width > boundSurface->variant->width ||
                variant->height > boundSurface->variant->height)
            {
                continue;
            }

            variant->deferredClearFlags |= flags;
            variant->deferredClearColor[0] = args.color[0];
            variant->deferredClearColor[1] = args.color[1];
            variant->deferredClearColor[2] = args.color[2];
            variant->deferredClearColor[3] = args.color[3];
            variant->deferredClearZ = args.z;
            variant->deferredClearStencil = args.stencil;
        }
    }
}
