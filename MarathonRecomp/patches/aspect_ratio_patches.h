#pragma once

#include <api/Marathon.h>
#include <xxHashMap.h>

static inline constexpr float NARROW_ASPECT_RATIO = 4.0f / 3.0f;
static inline constexpr float NARROW_ASPECT_RATIO_MAIN_MENU = 1072.0f / 720.0f;
static inline constexpr float WIDE_ASPECT_RATIO = 16.0f / 9.0f;

extern float g_aspectRatio;
extern float g_aspectRatioOffsetX;
extern float g_aspectRatioOffsetY;
extern float g_aspectRatioMultiplayerOffsetX;
extern float g_aspectRatioScale;
extern float g_aspectRatioGameplayScale;
extern float g_aspectRatioNarrowScale;
extern float g_aspectRatioNarrowMargin;
extern float g_horzCentre;
extern float g_vertCentre;
extern float g_radarMapScale;

extern std::array<float, 8> g_csdCorners;

class AspectRatioPatches
{
public:
    static void Init();
    static void ComputeOffsets();
    static float AlignX(float in_offset = 0.0f);
    static float AlignY(float in_offset = 0.0f);
};

enum MovieFlags : uint32_t
{
    MOVIE_CROP_NARROW = (1 << 0),
    MOVIE_CROP_WIDE = (1 << 1),
    MOVIE_CROP = MOVIE_CROP_NARROW | MOVIE_CROP_WIDE,

    MOVIE_BGM = (1 << 2)
};

struct MovieModifier
{
    uint32_t Flags{};
};

extern const xxHashMap<MovieModifier> g_movieModifiers;

void SetTextEntityModifier(Sonicteam::TextEntity* pTextEntity, uint64_t flags);
