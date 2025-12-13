#pragma once

#include <xxHashMap.h>

#define MAKE_BITFLAG32(bit) 1U << bit
#define MAKE_BITFLAG64(bit) 1ULL << bit

inline constexpr float NARROW_ASPECT_RATIO = 4.0f / 3.0f;
inline constexpr float WIDE_ASPECT_RATIO = 16.0f / 9.0f;
inline constexpr float STEAM_DECK_ASPECT_RATIO = 16.0f / 10.0f;

inline float g_aspectRatio;
inline float g_aspectRatioMovie;
inline float g_aspectRatioOffsetX;
inline float g_aspectRatioOffsetY;
inline float g_aspectRatioMultiplayerOffsetX;
inline float g_aspectRatioScale;
inline float g_aspectRatioGameplayScale;
inline float g_aspectRatioNarrowScale;
inline float g_aspectRatioNarrowMargin;
inline float g_horzCentre;
inline float g_vertCentre;
inline float g_radarMapScale;

class AspectRatioPatches
{
public:
    static void Init();
    static void ComputeOffsets();
};

// -------------- CSD MODIFIERS --------------- //

enum CsdFlags : uint64_t
{
    CSD_ALIGN_CENTER = 0,

    CSD_ALIGN_TOP = MAKE_BITFLAG64(0),
    CSD_ALIGN_LEFT = MAKE_BITFLAG64(1),
    CSD_ALIGN_BOTTOM = MAKE_BITFLAG64(2),
    CSD_ALIGN_RIGHT = MAKE_BITFLAG64(3),

    CSD_ALIGN_TOP_LEFT = CSD_ALIGN_TOP | CSD_ALIGN_LEFT,
    CSD_ALIGN_TOP_RIGHT = CSD_ALIGN_TOP | CSD_ALIGN_RIGHT,
    CSD_ALIGN_BOTTOM_LEFT = CSD_ALIGN_BOTTOM | CSD_ALIGN_LEFT,
    CSD_ALIGN_BOTTOM_RIGHT = CSD_ALIGN_BOTTOM | CSD_ALIGN_RIGHT,

    CSD_STRETCH_HORIZONTAL = MAKE_BITFLAG64(4),
    CSD_STRETCH_VERTICAL = MAKE_BITFLAG64(5),

    CSD_STRETCH = CSD_STRETCH_HORIZONTAL | CSD_STRETCH_VERTICAL,

    CSD_SCALE = MAKE_BITFLAG64(6),

    CSD_EXTEND_LEFT = MAKE_BITFLAG64(7),
    CSD_EXTEND_RIGHT = MAKE_BITFLAG64(8),

    CSD_STORE_LEFT_CORNER = MAKE_BITFLAG64(9),
    CSD_STORE_RIGHT_CORNER = MAKE_BITFLAG64(10),

    CSD_SKIP = MAKE_BITFLAG64(11),

    CSD_OFFSET_SCALE_LEFT = MAKE_BITFLAG64(12),
    CSD_OFFSET_SCALE_RIGHT = MAKE_BITFLAG64(13),

    CSD_REPEAT_LEFT = MAKE_BITFLAG64(14),
    CSD_REPEAT_RIGHT = MAKE_BITFLAG64(15),
    CSD_REPEAT_FLIP_HORIZONTAL = MAKE_BITFLAG64(16),
    CSD_REPEAT_FLIP_VERTICAL = MAKE_BITFLAG64(17),
    CSD_REPEAT_EXTEND = MAKE_BITFLAG64(18),

    CSD_UV_MODIFIER = MAKE_BITFLAG64(19),
    CSD_COLOUR_MODIFIER = MAKE_BITFLAG64(20),
    CSD_REPEAT_UV_MODIFIER = MAKE_BITFLAG64(21),
    CSD_REPEAT_COLOUR_MODIFIER = MAKE_BITFLAG64(22),

    CSD_BLACK_BAR = MAKE_BITFLAG64(23),
    CSD_PROHIBIT_BLACK_BAR = MAKE_BITFLAG64(24),

    CSD_UNSTRETCH_HORIZONTAL = MAKE_BITFLAG64(25),

    CSD_CORNER_EXTRACT = MAKE_BITFLAG64(26),

    CSD_RADARMAP = MAKE_BITFLAG64(27),

    CSD_POD_BASE = MAKE_BITFLAG64(28),
    CSD_POD_CLONE = MAKE_BITFLAG64(29),

    CSD_MULTIPLAYER = MAKE_BITFLAG64(30),

    CSD_CHEVRON = MAKE_BITFLAG64(31),

    CSD_MODIFIER_ULTRAWIDE_ONLY = MAKE_BITFLAG64(32),
    CSD_MODIFIER_NARROW_ONLY = MAKE_BITFLAG64(33),

    CSD_SCENE_DISABLE_MOTION = MAKE_BITFLAG64(34),

    CSD_MOVIE = MAKE_BITFLAG64(35),

    CSD_CRI_LOGO = MAKE_BITFLAG64(36),

    CSD_MAIN_MENU_PARTS_CAST_0221 = MAKE_BITFLAG64(37),
    CSD_MAIN_MENU_PARTS_CAST_0222 = MAKE_BITFLAG64(38),
    CSD_MAIN_MENU_PARTS_CAST_0226 = MAKE_BITFLAG64(39),
    CSD_MAIN_MENU_PARTS_CAST_0227 = MAKE_BITFLAG64(40),
    
    CSD_BUTTON_WINDOW = MAKE_BITFLAG64(41)
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
    int64_t Flags{};
    CsdUVs UVs{};
    CsdColours Colours{};
    CsdUVs RepeatUVs{};
    CsdColours RepeatColours{};
    float CornerMax{};
    uint32_t CornerIndex{};
};

extern const xxHashMap<CsdModifier> g_csdModifiers;

std::optional<CsdModifier> FindCsdModifier(uint32_t data);

// ------------- MOVIE MODIFIERS -------------- //

enum MovieFlags : uint32_t
{
    MOVIE_CROP_NARROW = MAKE_BITFLAG32(0),
    MOVIE_CROP_WIDE = MAKE_BITFLAG32(1),
    MOVIE_CROP = MOVIE_CROP_NARROW | MOVIE_CROP_WIDE
};

struct MovieModifier
{
    uint32_t Flags{};
};

extern const xxHashMap<MovieModifier> g_movieModifiers;

MovieModifier FindMovieModifier(XXH64_hash_t nameHash);

#undef MAKE_BITFLAG64
#undef MAKE_BITFLAG32
