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

inline XXH64_hash_t HashStr(const std::string_view& value)
{
    return XXH3_64bits(value.data(), value.size());
}

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

enum
{
    ALIGN_CENTER = 0 << 0,

    ALIGN_TOP = 1 << 0,
    ALIGN_LEFT = 1 << 1,
    ALIGN_BOTTOM = 1 << 2,
    ALIGN_RIGHT = 1 << 3,

    ALIGN_TOP_LEFT = ALIGN_TOP | ALIGN_LEFT,
    ALIGN_TOP_RIGHT = ALIGN_TOP | ALIGN_RIGHT,
    ALIGN_BOTTOM_LEFT = ALIGN_BOTTOM | ALIGN_LEFT,
    ALIGN_BOTTOM_RIGHT = ALIGN_BOTTOM | ALIGN_RIGHT,

    STRETCH_HORIZONTAL = 1 << 4,
    STRETCH_VERTICAL = 1 << 5,

    STRETCH = STRETCH_HORIZONTAL | STRETCH_VERTICAL,

    SCALE = 1 << 6,

    EXTEND_LEFT = 1 << 7,
    EXTEND_RIGHT = 1 << 8,

    STORE_LEFT_CORNER = 1 << 9,
    STORE_RIGHT_CORNER = 1 << 10,

    SKIP = 1 << 11,

    OFFSET_SCALE_LEFT = 1 << 12,
    OFFSET_SCALE_RIGHT = 1 << 13,

    REPEAT_LEFT = 1 << 14,
    REPEAT_RIGHT = 1 << 15,
    REPEAT_FLIP_HORIZONTAL = 1 << 16,
    REPEAT_FLIP_VERTICAL = 1 << 17,
    REPEAT_EXTEND = 1 << 18,

    UV_MODIFIER = 1 << 19,
    COLOUR_MODIFIER = 1 << 20,
    REPEAT_UV_MODIFIER = 1 << 21,
    REPEAT_COLOUR_MODIFIER = 1 << 22,

    PILLARBOX = 1 << 23,
    PROHIBIT_BLACK_BAR = 1 << 24,

    UNSTRETCH_HORIZONTAL = 1 << 25,

    CORNER_EXTRACT = 1 << 26,

    RADARMAP = 1 << 27,
    POD = 1 << 28
};

inline const xxHashMap<CsdModifier> g_modifiers =
{
    // audio
    { HashStr("sprite/audio/audio/pod/pod"), { POD } },
    { HashStr("sprite/audio/audio/pod/pod/Cast_1084"), { EXTEND_RIGHT } },
    { HashStr("sprite/audio/audio/pod/pod/Cast_1088"), { EXTEND_RIGHT } },

    // background
    { HashStr("sprite/background/background/mainmenu_back"), { STRETCH | PROHIBIT_BLACK_BAR } },

    // black_out
    { HashStr("sprite/black_out/black_out"), { STRETCH } },

    // bossname
    { HashStr("sprite/bossname/egg_cerberus/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1071/egg_cerberus/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1081/egg_cerberus/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/bossname/egg_genesis/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1091/egg_genesis/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/bossname/egg_wyvern/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1111/egg_wyvern/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/bossname/egg_wyvern/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1111/egg_wyvern/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/bossname/iblis/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1001/iblis/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1011/iblis/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1031/iblis/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/bossname/mephiles/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1041/mephiles/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1061/mephiles/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/bossname/shadow_the_hedgehog/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1161/shadow_the_hedgehog/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/bossname/silver_the_hedgehog/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1151/silver_the_hedgehog/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/bossname/solaris/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1121/solaris/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/bossname/sonic_the_hedgehog/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("event/e1141/sonic_the_hedgehog/Scene_0000"), { ALIGN_BOTTOM_RIGHT | SCALE } },

    // button_window
    { HashStr("sprite/button_window/button_window/Scene_0000"), { ALIGN_BOTTOM | SCALE } },
    { HashStr("sprite/button_window/button_window/Scene_0000/Null_0000/Cast_0002"), { EXTEND_RIGHT } },

    // gadget_ber
    { HashStr("sprite/gadget_ber/gadget_bar/gadgetbar"), { ALIGN_BOTTOM_RIGHT } },
    { HashStr("sprite/gadget_ber/gadget_bar/gadgetbar_anime"), { ALIGN_BOTTOM_RIGHT } },
    { HashStr("sprite/gadget_ber/gadget_bar/gadgetbar_ue"), { ALIGN_BOTTOM_RIGHT } },
    { HashStr("sprite/gadget_ber/gadget_bar/icon_text"), { ALIGN_BOTTOM_RIGHT } },

    // cri_logo
    { HashStr("sprite/logo/cri_logo/Scene_0000/Null_0002/bg"), { STRETCH } },
    { HashStr("sprite/logo/cri_logo/Scene_0000/Null_0002/criware"), { SCALE } },

    // loading_english
    // TODO: draw pillarbox.
    { HashStr("sprite/loading/loading_english/Scene_0000/Loading"), { ALIGN_BOTTOM } },
    { HashStr("sprite/loading/loading_english/Scene_0000/Loading_02"), { ALIGN_BOTTOM } },
    { HashStr("sprite/loading/loading_english/Scene_0000/arrow_01"), { ALIGN_BOTTOM } },
    { HashStr("sprite/loading/loading_english/Scene_0000/arrow_02"), { ALIGN_BOTTOM } },
    { HashStr("sprite/loading/loading_english/Scene_0000/arrow_03"), { ALIGN_BOTTOM } },

    // main_menu
    { HashStr("sprite/main_menu/savedate/savedata/Null_1074/sita3"), { EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/savedate/savedata/Null_1074/sita5"), { EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/savedate/savedata/Null_1074/sita8"), { EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/stage_plate/stage_plate/stage_plate2"), { EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/stage_plate/stage_plate/Cast_1337"), { EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/stage_plate/stage_plate/Cast_1340"), { EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/mission_plate/mission_plate/mission_plate2"), { EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/mission_plate/mission_plate/Cast_1332"), { EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/mission_plate/mission_plate/Cast_1336"), { EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/text"), { ALIGN_BOTTOM | SCALE } },
    { HashStr("sprite/main_menu/text_cover/Null_0290/cover_l"), { ALIGN_BOTTOM | SCALE | UV_MODIFIER | REPEAT_LEFT | REPEAT_FLIP_HORIZONTAL | REPEAT_EXTEND | REPEAT_UV_MODIFIER, { 0.0015f, 0, 0.0015f, 0, 0, 0, 0, 0 }, {}, { 0, 0, 0, 0, -0.05f, 0, -0.05f, 0 } } },
    { HashStr("sprite/main_menu/text_cover/Null_0290/cover_c"), { ALIGN_BOTTOM | SCALE } },
    { HashStr("sprite/main_menu/text_cover/Null_0290/vocer_r"), { ALIGN_BOTTOM | SCALE | UV_MODIFIER | REPEAT_RIGHT | REPEAT_FLIP_HORIZONTAL | REPEAT_EXTEND | REPEAT_UV_MODIFIER, { 0, 0, 0, 0, 0.0015f, 0, 0.0015f, 0 }, {}, { -0.05f, 0, -0.05f, 0, 0, 0, 0, 0 } } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0218/Cast_0221"), { ALIGN_TOP | SCALE | EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0218/Cast_0222"), { ALIGN_TOP | SCALE | UV_MODIFIER | REPEAT_LEFT | REPEAT_FLIP_HORIZONTAL | REPEAT_EXTEND | REPEAT_UV_MODIFIER, { 0.0015f, 0, 0.0015f, 0, 0, 0, 0, 0 }, {}, { 0.1f, 0, 0.1f, 0, -0.1f, 0, -0.1f, 0 } } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0960/Cast_0964"), { ALIGN_TOP | SCALE | EXTEND_RIGHT } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0960/Cast_0965"), { ALIGN_TOP | SCALE | REPEAT_LEFT | REPEAT_FLIP_HORIZONTAL | REPEAT_EXTEND | REPEAT_UV_MODIFIER, {}, {}, { 0, 0, 0, 0, -0.5f, 0, -0.5f, 0 } } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0224/Cast_0226"), { ALIGN_BOTTOM | SCALE | REPEAT_LEFT | REPEAT_FLIP_HORIZONTAL | REPEAT_EXTEND | REPEAT_UV_MODIFIER, {}, {}, { 0, 0, 0, 0, -0.8f, 0, -0.8f, 0 } } },
    { HashStr("sprite/main_menu/main_menu_parts/Null_0224/Cast_0227"), { ALIGN_BOTTOM | SCALE | REPEAT_RIGHT | REPEAT_FLIP_HORIZONTAL | REPEAT_EXTEND | REPEAT_UV_MODIFIER, {}, {}, { -0.8f, 0, -0.8f, 0, 0, 0, 0, 0 } } },

    // maindisplay
    { HashStr("sprite/maindisplay/power"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/maindisplay/custom_bar_anime"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/maindisplay/power_a"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/maindisplay/power_bar_anime"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/maindisplay/score"), { ALIGN_TOP_LEFT | SCALE } },
    { HashStr("sprite/maindisplay/life_ber_anime"), { ALIGN_TOP_LEFT | SCALE } },
    { HashStr("sprite/maindisplay/life"), { ALIGN_TOP_LEFT | SCALE } },
    { HashStr("sprite/maindisplay/time"), { ALIGN_TOP_LEFT | SCALE } },
    { HashStr("sprite/maindisplay/ring"), { ALIGN_TOP_LEFT | SCALE } },
    { HashStr("sprite/maindisplay/ring_anime"), { ALIGN_TOP_LEFT | SCALE } },
    { HashStr("sprite/maindisplay/bar_ue"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/maindisplay/power_bar_effect"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/maindisplay/ring_000_effect"), { ALIGN_TOP_LEFT | SCALE } },
    { HashStr("sprite/maindisplay/boss_gauge"), { ALIGN_TOP_RIGHT | SCALE } },
    { HashStr("sprite/maindisplay/boss_gauge_anime"), { ALIGN_TOP_RIGHT | SCALE } },
    { HashStr("sprite/maindisplay/item_ber_anime"), { ALIGN_TOP_LEFT | SCALE } },
    { HashStr("sprite/maindisplay/item"), { ALIGN_TOP_LEFT | SCALE } },
    { HashStr("sprite/maindisplay/custom_gem"), { ALIGN_BOTTOM_RIGHT | SCALE } },
    { HashStr("sprite/maindisplay/custom_level"), { ALIGN_BOTTOM_RIGHT | SCALE } },

    // radarmap_cover
    { HashStr("sprite/radarmap_cover/radarmap_cover/Scene_0000"), { RADARMAP | ALIGN_TOP_RIGHT | SCALE } },

    // result_English
    { HashStr("sprite/result/result_English/title_plate"), { PILLARBOX } }, // TODO

    // sonicteam_logo
    { HashStr("sprite/logo/sonicteam_logo/sonicteam"), { SCALE } },

    // tag_character
    { HashStr("sprite/tag_character/tag_character/1p_tug/1p_tug/1p_tug1"), { REPEAT_LEFT | REPEAT_FLIP_HORIZONTAL | REPEAT_EXTEND | REPEAT_UV_MODIFIER | REPEAT_COLOUR_MODIFIER, {}, {}, { 0, 0, 0, 0, -0.5f, 0, -0.5f, 0 }, { 0xFFFFFF50, 0xFFFFFF50, 0xFFFFFF50, 0xFFFFFF50 } } },
    { HashStr("sprite/tag_character/tag_character/2p_tug/2p_tug/2p_tug1"), { REPEAT_RIGHT | REPEAT_FLIP_HORIZONTAL | REPEAT_EXTEND | REPEAT_UV_MODIFIER | REPEAT_COLOUR_MODIFIER, {}, {}, { 0, 0, 0, 0, -0.5f, 0, -0.5f, 0 }, { 0xFFFFFF50, 0xFFFFFF50, 0xFFFFFF50, 0xFFFFFF50 } } },

    // talkwindow
    { HashStr("sprite/talkwindow/talkwindow/window"), { SCALE } },
    { HashStr("sprite/talkwindow/talkwindow/nafuda"), { SCALE } },
    { HashStr("sprite/talkwindow/talkwindow/Scene_0021"), { SCALE } },

    // title_English
    { HashStr("sprite/title/title_English/Scene_Title/Logo_add"), { SCALE } },
    { HashStr("sprite/title/title_English/Scene_Title/Logo"), { SCALE } },
    { HashStr("sprite/title/title_English/Scene_Title/copyright"), { ALIGN_BOTTOM } },

    // towndisplay
    { HashStr("sprite/towndisplay/ring"), { ALIGN_TOP_LEFT | SCALE } },
    { HashStr("sprite/towndisplay/ring_anime"), { ALIGN_TOP_LEFT | SCALE } },

    // trickpoint_English
    // TODO: offset score text properly.
    { HashStr("sprite/trickpoint/trickpoint_English/Scene_0000"), { ALIGN_TOP_RIGHT } },
    { HashStr("sprite/trickpoint/trickpoint_English/Scene_0001"), { ALIGN_TOP_RIGHT } },
    { HashStr("sprite/trickpoint/trickpoint_English/score"), { ALIGN_TOP_RIGHT } },
};

struct TextFontPictureParams
{
    uint16_t X{};
    uint16_t Y{};
    uint16_t Width{};
    uint16_t Height{};
};

inline const xxHashMap<TextFontPictureParams> g_pftParamsXenon =
{
    { HashStr("button_a"), { 0, 0, 28, 28 } },
    { HashStr("button_b"), { 28, 0, 28, 28 } },
    { HashStr("button_x"), { 56, 0, 28, 28 } },
    { HashStr("button_y"), { 84, 0, 28, 28 } },
    { HashStr("button_lb"), { 112, 0, 53, 28 } },
    { HashStr("button_lt"), { 56, 28, 55, 28 } },
    { HashStr("button_rb"), { 168, 0, 53, 28 } },
    { HashStr("button_rt"), { 0, 28, 55, 28 } },
    { HashStr("button_start"), { 112, 28, 28, 28 } },
    { HashStr("button_back"), { 140, 28, 28, 28 } }
};

inline const xxHashMap<TextFontPictureParams> g_pftParamsPS3 =
{
    { HashStr("button_a"), { 0, 56, 28, 28 } },
    { HashStr("button_b"), { 28, 56, 28, 28 } },
    { HashStr("button_x"), { 56, 56, 28, 28 } },
    { HashStr("button_y"), { 84, 56, 28, 28 } },
    { HashStr("button_lb"), { 112, 56, 48, 28 } },
    { HashStr("button_lt"), { 168, 56, 48, 28 } },
    { HashStr("button_rb"), { 0, 84, 48, 28 } },
    { HashStr("button_rt"), { 56, 84, 48, 28 } },
    { HashStr("button_start"), { 140, 84, 28, 28 } },
    { HashStr("button_back"), { 112, 84, 28, 28 } }
};

inline TextFontPictureParams FindFontPictureParams(xxHashMap<TextFontPictureParams> pftParams, std::string_view& name)
{
    auto findResult = pftParams.find(HashStr(name));

    if (findResult != pftParams.end())
        return findResult->second;

    return {};
}
