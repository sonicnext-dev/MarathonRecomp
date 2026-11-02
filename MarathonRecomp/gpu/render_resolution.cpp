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
            return 0.5f;
        case EReflectionResolution::Half:
            return 2.0f;
        case EReflectionResolution::Full:
            return 4.0f;
        default:
            return 1.0f;
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

    if (name == "framebuffer0" || name == "framebuffer1")
        return internalSize;

    // Downsampled chain, e.g. "framebuffer_1_4_0" is a quarter of the internal resolution.
    if (name.starts_with("framebuffer_1_"))
    {
        uint32_t divisor = 0;
        std::from_chars(name.data() + 14, name.data() + name.size(), divisor);

        if (divisor != 0)
            return BufferSize{ internalSize.Width / divisor, internalSize.Height / divisor };
    }

    // Reflections are an eighth of the internal resolution at the default setting.
    if (name == "reflection0" || name == "depthstencil_1_4")
        return BufferSize{ ScaleDimension(internalSize.Width >> 3, GetReflectionScale()), ScaleDimension(internalSize.Height >> 3, GetReflectionScale()) };

    if (name == "radermap" || blockName == "radermap0")
        return BufferSize{ uint32_t(g_radarMapScale), uint32_t(g_radarMapScale) };

    const auto shadowResolution = uint32_t(Config::ShadowResolution.Value);

    if (shadowResolution > 0)
    {
        // RenderMefiress
        if (name == "user0" || (blockName == "user0" && name == "depthstencil_256"))
            return BufferSize{ shadowResolution, shadowResolution };

        if (name == "csm")
            return BufferSize{ shadowResolution, shadowResolution };
    }

    return {};
}
