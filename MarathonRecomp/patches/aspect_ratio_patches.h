#pragma once

#include <xxHashMap.h>

inline constexpr float NARROW_ASPECT_RATIO = 4.0f / 3.0f;
inline constexpr float WIDE_ASPECT_RATIO = 16.0f / 9.0f;
inline constexpr float STEAM_DECK_ASPECT_RATIO = 16.0f / 10.0f;

inline float g_aspectRatio;
inline float g_aspectRatioOffsetX;
inline float g_aspectRatioOffsetY;
inline float g_aspectRatioScale;
inline float g_aspectRatioGameplayScale;
inline float g_aspectRatioNarrowScale;

struct AspectRatioPatches
{
    static void ComputeOffsets();
};

inline float ComputeScale(float aspectRatio)
{
    return ((aspectRatio * 720.0f) / 1280.0f) / sqrt((aspectRatio * 720.0f) / 1280.0f);
}

enum MovieFlags : uint32_t
{
    CROP_NARROW = 1 << 0,
    CROP_WIDE = 1 << 1,
    CROP = CROP_NARROW | CROP_WIDE
};

struct MovieModifier
{
    MovieFlags Flags{};
};

extern const xxHashMap<MovieModifier> g_movieModifiers;

inline MovieModifier FindMovieModifier(XXH64_hash_t nameHash)
{
    auto findResult = g_movieModifiers.find(nameHash);

    if (findResult != g_movieModifiers.end())
        return findResult->second;

    return {};
}
