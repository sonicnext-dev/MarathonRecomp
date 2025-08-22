#include "aspect_ratio_patches.h"
#include <api/Marathon.h>
#include <gpu/video.h>
#include <user/config.h>
#include <app.h>

static float ComputeScale(float aspectRatio)
{
    return ((aspectRatio * 720.0f) / 1280.0f) / sqrt((aspectRatio * 720.0f) / 1280.0f);
}

void AspectRatioPatches::ComputeOffsets()
{
    float width = Video::s_viewportWidth;
    float height = Video::s_viewportHeight;

    g_aspectRatio = width / height;
    g_aspectRatioGameplayScale = 1.0f;

    if (g_aspectRatio >= NARROW_ASPECT_RATIO)
    {
        g_aspectRatioOffsetX = (width - height * WIDE_ASPECT_RATIO) / 2.0f;
        g_aspectRatioOffsetY = 0.0f;
        g_aspectRatioScale = height / 720.0f;

        // keep same scale above Steam Deck aspect ratio
        if (g_aspectRatio < WIDE_ASPECT_RATIO)
        {
            // interpolate to original 4:3 scale
            float steamDeckScale = g_aspectRatio / WIDE_ASPECT_RATIO;
            float narrowScale = ComputeScale(NARROW_ASPECT_RATIO);
            float lerpFactor = std::clamp((g_aspectRatio - NARROW_ASPECT_RATIO) / (STEAM_DECK_ASPECT_RATIO - NARROW_ASPECT_RATIO), 0.0f, 1.0f);

            g_aspectRatioGameplayScale = narrowScale + (steamDeckScale - narrowScale) * lerpFactor;
        }
    }
    else
    {
        // 4:3 crop
        g_aspectRatioOffsetX = (width - width * NARROW_ASPECT_RATIO) / 2.0f;
        g_aspectRatioOffsetY = (height - width / NARROW_ASPECT_RATIO) / 2.0f;
        g_aspectRatioScale = width / 960.0f;
        g_aspectRatioGameplayScale = ComputeScale(NARROW_ASPECT_RATIO);
    } 

    g_aspectRatioNarrowScale = std::clamp((g_aspectRatio - NARROW_ASPECT_RATIO) / (WIDE_ASPECT_RATIO - NARROW_ASPECT_RATIO), 0.0f, 1.0f);
}

// Sonicteam::MovieObjectWmv::Draw
PPC_FUNC_IMPL(__imp__sub_8264CC90);
PPC_FUNC(sub_8264CC90)
{
    auto pMovieObjectWmv = (Sonicteam::MovieObjectWmv*)(base + ctx.r3.u32);

    static XXH64_hash_t s_movieNameHash{};
    static bool s_movieUsesCustomDimensions{};
    static float s_movieLeft{};
    static float s_movieRight{};
    static float s_movieTop{};
    static float s_movieBottom{};

    auto movieNameHash = HashStr(pMovieObjectWmv->m_FilePath.c_str());

    if (movieNameHash != s_movieNameHash)
    {
        s_movieNameHash = movieNameHash;
        s_movieUsesCustomDimensions = pMovieObjectWmv->m_UseCustomDimensions;
        s_movieLeft = pMovieObjectWmv->m_Left;
        s_movieRight = pMovieObjectWmv->m_Right;
        s_movieTop = pMovieObjectWmv->m_Top;
        s_movieBottom = pMovieObjectWmv->m_Bottom;

        LOGFN_UTILITY("Movie: {} - {}x{}", pMovieObjectWmv->m_FilePath.c_str(), pMovieObjectWmv->m_Width.get(), pMovieObjectWmv->m_Height.get());
    }

    auto movieModifier = FindMovieModifier(movieNameHash);
    auto movieAspectRatio = (float)pMovieObjectWmv->m_Width / (float)pMovieObjectWmv->m_Height;

    float width, height, left, right, top, bottom;

    if (s_movieUsesCustomDimensions)
    {
        auto movieRectLeft = s_movieLeft / (g_aspectRatio / movieAspectRatio);
        auto movieRectRight = s_movieRight / (g_aspectRatio / movieAspectRatio);
        auto movieRectWidth = movieRectRight - movieRectLeft;
        auto movieRectHeight = s_movieTop - s_movieBottom;
        auto movieRectCentreX = (movieRectLeft + movieRectRight) / 2.0f;
        auto movieRectCentreY = (s_movieTop + s_movieBottom) / 2.0f;

        width = movieRectWidth;
        height = movieRectHeight;
    
        left = movieRectCentreX - (width / 2.0f);
        right = movieRectCentreX + (width / 2.0f);
        top = movieRectCentreY + (height / 2.0f);
        bottom = movieRectCentreY - (height / 2.0f);
    }
    else
    {
        auto shouldCrop = (movieModifier.Flags & MovieFlags::CROP) != 0;

        width = 2.0f;
        height = 2.0f;

        if (g_aspectRatio > movieAspectRatio)
        {
            shouldCrop = (movieModifier.Flags & MovieFlags::CROP_WIDE) != 0;

            if (shouldCrop)
            {
                // Crop vertically at wide aspect ratios.
                height = 2.0f * (g_aspectRatio / movieAspectRatio);
            }
            else
            {
                // Pillarbox at wide aspect ratios.
                width = 2.0f * (movieAspectRatio / g_aspectRatio);
            }
        }
        else
        {
            shouldCrop = (movieModifier.Flags & MovieFlags::CROP_NARROW) != 0;

            if (shouldCrop)
            {
                // Crop horizontally at narrow aspect ratios.
                width = 2.0f * (movieAspectRatio / g_aspectRatio);
            }
            else
            {
                // Letterbox at narrow aspect ratios.
                height = 2.0f * (g_aspectRatio / movieAspectRatio);
            }
        }

        left = -width / 2.0f;
        right = (width / 2.0f) * 2.0f;
        top = height / 2.0f;
        bottom = (-height / 2.0f) * 2.0f;
    }

    pMovieObjectWmv->m_UseCustomDimensions = true;
    pMovieObjectWmv->m_Left = left;
    pMovieObjectWmv->m_Right = right;
    pMovieObjectWmv->m_Top = top;
    pMovieObjectWmv->m_Bottom = bottom;

    __imp__sub_8264CC90(ctx, base);
}
