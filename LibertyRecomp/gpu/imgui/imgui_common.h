#pragma once

#define IMGUI_SHADER_MODIFIER_NONE                     0
#define IMGUI_SHADER_MODIFIER_TEXT_SKEW                1
#define IMGUI_SHADER_MODIFIER_HORIZONTAL_MARQUEE_FADE  2
#define IMGUI_SHADER_MODIFIER_VERTICAL_MARQUEE_FADE    3
#define IMGUI_SHADER_MODIFIER_GRAYSCALE                4
#define IMGUI_SHADER_MODIFIER_TITLE_BEVEL              5
#define IMGUI_SHADER_MODIFIER_CATEGORY_BEVEL           6
#define IMGUI_SHADER_MODIFIER_RECTANGLE_BEVEL          7
#define IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT         8

#if defined(__cplusplus) && !defined(__air__)

enum class ImGuiCallback : int32_t
{
    SetGradient = -1,
    SetShaderModifier = -2,
    SetOrigin = -3,
    SetScale = -4,
    SetMarqueeFade = -5,
    SetOutline = -6,
    SetProceduralOrigin = -7,
    // -8 is ImDrawCallback_ResetRenderState, don't use!
    SetAdditive = -9
};

union ImGuiCallbackData
{
    struct
    {
        float boundsMin[2];
        float boundsMax[2];
        uint32_t gradientTopLeft;
        uint32_t gradientTopRight;
        uint32_t gradientBottomRight;
        uint32_t gradientBottomLeft;
    } setGradient;

    struct
    {
        uint32_t shaderModifier;
    } setShaderModifier;

    struct
    {
        float origin[2];
    } setOrigin;

    struct
    {
        float scale[2];
    } setScale;

    struct
    {
        float boundsMin[2];
        float boundsMax[2];
    } setMarqueeFade;

    struct
    {
        float outline;
    } setOutline;

    struct
    {
        float proceduralOrigin[2];
    } setProceduralOrigin;

    struct
    {
        bool enabled;
    } setAdditive;
};

extern ImGuiCallbackData* AddImGuiCallback(ImGuiCallback callback);

extern void ResetImGuiCallbacks();

#endif
