// GTA IV-specific aspect ratio patches
// This is a minimal implementation replacing the Sonic '06 specific version

#include "aspect_ratio_patches.h"
#include <gpu/video.h>

// Empty modifier maps for now - GTA IV will need its own
const xxHashMap<CsdModifier> g_csdModifiers{};
const xxHashMap<MovieModifier> g_movieModifiers{};

std::optional<CsdModifier> FindCsdModifier(uint32_t data)
{
    auto it = g_csdModifiers.find(data);
    if (it != g_csdModifiers.end())
        return it->second;
    return std::nullopt;
}

MovieModifier FindMovieModifier(XXH64_hash_t nameHash)
{
    auto it = g_movieModifiers.find(nameHash);
    if (it != g_movieModifiers.end())
        return it->second;
    return {};
}

void AspectRatioPatches::Init()
{
    // Compute initial offsets
    ComputeOffsets();
}

void AspectRatioPatches::ComputeOffsets()
{
    // Get current resolution
    auto width = static_cast<float>(Video::s_viewportWidth);
    auto height = static_cast<float>(Video::s_viewportHeight);
    
    // Fallback if dimensions not set yet
    if (width == 0 || height == 0)
    {
        width = 1280.0f;
        height = 720.0f;
    }
    
    // Calculate aspect ratio
    g_aspectRatio = width / height;
    g_aspectRatioMovie = g_aspectRatio;
    
    // Calculate scaling and offsets
    // GTA IV original resolution: 1280x720 (16:9)
    constexpr float ORIGINAL_WIDTH = 1280.0f;
    constexpr float ORIGINAL_HEIGHT = 720.0f;
    
    g_aspectRatioScale = height / ORIGINAL_HEIGHT;
    
    if (g_aspectRatio >= WIDE_ASPECT_RATIO)
    {
        // Wider than 16:9 - pillarbox
        float scaledWidth = ORIGINAL_WIDTH * g_aspectRatioScale;
        g_aspectRatioOffsetX = (width - scaledWidth) / 2.0f;
        g_aspectRatioOffsetY = 0.0f;
    }
    else
    {
        // Narrower than 16:9 - letterbox
        float targetWidth = height * WIDE_ASPECT_RATIO;
        g_aspectRatioScale = width / ORIGINAL_WIDTH;
        float scaledHeight = ORIGINAL_HEIGHT * g_aspectRatioScale;
        g_aspectRatioOffsetX = 0.0f;
        g_aspectRatioOffsetY = (height - scaledHeight) / 2.0f;
    }
    
    g_aspectRatioGameplayScale = g_aspectRatioScale;
    g_aspectRatioNarrowScale = 1.0f;
    g_aspectRatioNarrowMargin = 0.0f;
    g_aspectRatioMultiplayerOffsetX = 0.0f;
    
    g_horzCentre = width / 2.0f;
    g_vertCentre = height / 2.0f;
    
    g_radarMapScale = g_aspectRatioScale;
}
