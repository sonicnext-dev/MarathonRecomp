#include "render_resolution.h"
#include <gpu/video.h>
#include <patches/aspect_ratio_patches.h>
#include <user/config.h>

static uint32_t ScaleDimension(uint32_t value, float scale)
{
    return uint32_t(float(value) * scale);
}

static float GetReflectionScale()
{
    switch (Config::ReflectionResolution)
    {
        case EReflectionResolution::Eighth:
            return 0.125f;
        case EReflectionResolution::Quarter:
            return 0.25f;
        case EReflectionResolution::Half:
            return 0.5f;
        case EReflectionResolution::Full:
            return 1.0f;
        default:
            assert(false && "Unimplemented reflection resolution");
            return 0.25f;
    }
}

static uint32_t GetShadowResolution()
{
    switch (Config::ShadowResolution)
    {
        case EShadowResolution::x512:
            return 512;
        case EShadowResolution::x1024:
            return 1024;
        case EShadowResolution::x2048:
            return 2048;
        case EShadowResolution::x4096:
            return 4096;
        case EShadowResolution::x8192:
            return 8192;
        default:
            assert(false && "Unimplemented shadow resolution");
            return 1024;
    }
}

BufferSize RenderResolution::GetInternalSize()
{
    return { ScaleDimension(Video::s_viewportWidth, Config::ResolutionScale), ScaleDimension(Video::s_viewportHeight, Config::ResolutionScale) };
}

std::optional<BufferSize> RenderResolution::FindBufferSize(std::string_view name, const char* pBlockName)
{
    const auto internalSize = GetInternalSize();
    const auto blockName = std::string_view(pBlockName != nullptr ? pBlockName : "");

    if (name == "framebuffer0" || name == "framebuffer1" || name == "framebuffer_hdr")
        return internalSize;

    if (name.starts_with("coframebuffer") || name == "codepthstencil")
        return BufferSize { internalSize.Width / 2, internalSize.Height };

    // Downsampled chain, e.g. "framebuffer_1_4_0" is a quarter of the internal resolution.
    if (name.starts_with("framebuffer_1_"))
    {
        uint32_t divisor = 0;
        std::from_chars(name.data() + 14, name.data() + name.size(), divisor);

        if (divisor != 0)
            return BufferSize { Video::s_viewportWidth / divisor, Video::s_viewportHeight / divisor };
    }

    if (name == "radermap" || blockName == "radermap0")
        return BufferSize { uint32_t(g_radarMapScale), uint32_t(g_radarMapScale) };

    if (name == "reflection0" || name == "depthstencil_1_4")
    {
        const auto reflectionScale = GetReflectionScale();
        return BufferSize { ScaleDimension(internalSize.Width, reflectionScale), ScaleDimension(internalSize.Height, reflectionScale) };
    }

    // RenderMefiress uses "user0" for Mephiles' shadow form
    if (name == "csm" || name == "user0" || (blockName == "user0" && name == "depthstencil_256"))
    {
        const auto shadowResolution = GetShadowResolution();
        return BufferSize { shadowResolution, shadowResolution };
    }

    return {};
}
