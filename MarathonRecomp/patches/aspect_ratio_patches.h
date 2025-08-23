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

class AspectRatioPatches
{
public:
    static void Init();
    static void ComputeOffsets();
};

inline float ComputeScale(float aspectRatio)
{
    return ((aspectRatio * 720.0f) / 1280.0f) / sqrt((aspectRatio * 720.0f) / 1280.0f);
}

// -------------- CSD --------------- //

enum CsdFlags
{
    CSD_ALIGN_CENTER = 0 << 0,

    CSD_ALIGN_TOP = 1 << 0,
    CSD_ALIGN_LEFT = 1 << 1,
    CSD_ALIGN_BOTTOM = 1 << 2,
    CSD_ALIGN_RIGHT = 1 << 3,

    CSD_ALIGN_TOP_LEFT = CSD_ALIGN_TOP | CSD_ALIGN_LEFT,
    CSD_ALIGN_TOP_RIGHT = CSD_ALIGN_TOP | CSD_ALIGN_RIGHT,
    CSD_ALIGN_BOTTOM_LEFT = CSD_ALIGN_BOTTOM | CSD_ALIGN_LEFT,
    CSD_ALIGN_BOTTOM_RIGHT = CSD_ALIGN_BOTTOM | CSD_ALIGN_RIGHT,

    CSD_STRETCH_HORIZONTAL = 1 << 4,
    CSD_STRETCH_VERTICAL = 1 << 5,

    CSD_STRETCH = CSD_STRETCH_HORIZONTAL | CSD_STRETCH_VERTICAL,

    CSD_SCALE = 1 << 6,

    CSD_EXTEND_LEFT = 1 << 7,
    CSD_EXTEND_RIGHT = 1 << 8,

    CSD_STORE_LEFT_CORNER = 1 << 9,
    CSD_STORE_RIGHT_CORNER = 1 << 10,

    CSD_SKIP = 1 << 11,

    CSD_OFFSET_SCALE_LEFT = 1 << 12,
    CSD_OFFSET_SCALE_RIGHT = 1 << 13,

    CSD_REPEAT_LEFT = 1 << 14,
    CSD_REPEAT_RIGHT = 1 << 15,
    CSD_REPEAT_FLIP_HORIZONTAL = 1 << 16,
    CSD_REPEAT_FLIP_VERTICAL = 1 << 17,
    CSD_REPEAT_EXTEND = 1 << 18,

    CSD_UV_MODIFIER = 1 << 19,
    CSD_COLOUR_MODIFIER = 1 << 20,
    CSD_REPEAT_UV_MODIFIER = 1 << 21,
    CSD_REPEAT_COLOUR_MODIFIER = 1 << 22,

    CSD_PILLARBOX = 1 << 23,
    CSD_PROHIBIT_BLACK_BAR = 1 << 24,

    CSD_UNSTRETCH_HORIZONTAL = 1 << 25,

    CSD_CORNER_EXTRACT = 1 << 26,

    CSD_RADARMAP = 1 << 27,
    CSD_POD = 1 << 28,

    CSD_MULTIPLAYER = 1 << 29,
    CSD_MULTIPLAYER_CENTER = 1 << 30
};

struct CsdUVs
{
    float U0{};
    float V0{};
    float U1{};
    float V1{};
    float U2{};
    float V2{};
    float U3{};
    float V3{};
};

struct CsdColours
{
    uint32_t C0{};
    uint32_t C1{};
    uint32_t C2{};
    uint32_t C3{};
};

struct CsdModifier
{
    uint32_t Flags{};
    CsdUVs UVs{};
    CsdColours Colours{};
    CsdUVs RepeatUVs{};
    CsdColours RepeatColours{};
    float CornerMax{};
    uint32_t CornerIndex{};
};

extern const xxHashMap<CsdModifier> g_csdModifiers;

std::optional<CsdModifier> FindCsdModifier(uint32_t data);

// -------------- TEXT -------------- //

struct TextFontPictureModifier
{
    uint16_t X{};
    uint16_t Y{};
    uint16_t Width{};
    uint16_t Height{};
};

extern const xxHashMap<TextFontPictureModifier> g_pftModifierXenon;
extern const xxHashMap<TextFontPictureModifier> g_pftModifierPS3;

TextFontPictureModifier FindFontPictureModifier(xxHashMap<TextFontPictureModifier> pftModifier, std::string_view& name);

// ------------- MOVIE -------------- //

enum MovieFlags
{
    MOVIE_CROP_NARROW = 1 << 0,
    MOVIE_CROP_WIDE = 1 << 1,
    MOVIE_CROP = MOVIE_CROP_NARROW | MOVIE_CROP_WIDE
};

struct MovieModifier
{
    uint32_t Flags{};
};

extern const xxHashMap<MovieModifier> g_movieModifiers;

MovieModifier FindMovieModifier(XXH64_hash_t nameHash);
