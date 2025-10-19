#include "button_guide.h"
#include <gpu/imgui/imgui_snapshot.h>
#include <gpu/video.h>
#include <hid/hid.h>
#include <patches/aspect_ratio_patches.h>
#include <res/images/common/button_window.dds.h>
#include <res/images/common/controller.dds.h>
#include <res/images/common/kbm.dds.h>
#include <ui/imgui_utils.h>
#include <user/config.h>
#include <app.h>
#include <decompressor.h>

static std::vector<Button> g_buttons{};

static double g_time{};
static bool g_isAnimated{};

static std::unique_ptr<GuestTexture> g_upTexButtonWindow{};
static std::unique_ptr<GuestTexture> g_upTexController{};
static std::unique_ptr<GuestTexture> g_upTexKbm{};

struct ButtonDimensions
{
    uint16_t X;
    uint16_t Y;
    uint16_t Width;
    uint16_t Height;
};

static std::unordered_map<EButtonIcon, ButtonDimensions> g_iconsXenon =
{
    { EButtonIcon::A, { 0, 0, 28, 28 } },
    { EButtonIcon::B, { 28, 0, 28, 28 } },
    { EButtonIcon::X, { 56, 0, 28, 28 } },
    { EButtonIcon::Y, { 84, 0, 28, 28 } },
    { EButtonIcon::LB, { 112, 0, 53, 28 } },
    { EButtonIcon::RB, { 168, 0, 53, 28 } },
    { EButtonIcon::LBRB, { 0, 0, 106, 28 } },
    { EButtonIcon::LT, { 56, 28, 55, 28 } },
    { EButtonIcon::RT, { 0, 28, 55, 28 } },
    { EButtonIcon::LTRT, { 0, 0, 110, 28 } },
    { EButtonIcon::Start, { 112, 28, 28, 28 } },
    { EButtonIcon::Back, { 140, 28, 28, 28 } },
    { EButtonIcon::LMB, { 0, 0, 128, 128 } },     // TODO
    { EButtonIcon::Enter, { 128, 0, 128, 128 } }, // TODO
    { EButtonIcon::Escape, { 256, 0, 128, 128 } } // TODO
};

static std::unordered_map<EButtonIcon, ButtonDimensions> g_iconsPS3 =
{
    { EButtonIcon::A, { 0, 56, 28, 28 } },
    { EButtonIcon::B, { 28, 56, 28, 28 } },
    { EButtonIcon::X, { 56, 56, 28, 28 } },
    { EButtonIcon::Y, { 84, 56, 28, 28 } },
    { EButtonIcon::LB, { 112, 56, 48, 28 } },
    { EButtonIcon::RB, { 0, 84, 48, 28 } },
    { EButtonIcon::LBRB, { 0, 0, 96, 28 } },
    { EButtonIcon::LT, { 168, 56, 48, 28 } },
    { EButtonIcon::RT, { 56, 84, 48, 28 } },
    { EButtonIcon::LTRT, { 0, 0, 96, 28 } },
    { EButtonIcon::Start, { 140, 84, 28, 28 } },
    { EButtonIcon::Back, { 112, 84, 28, 28 } },
    { EButtonIcon::LMB, { 0, 0, 128, 128 } },     // TODO
    { EButtonIcon::Enter, { 128, 0, 128, 128 } }, // TODO
    { EButtonIcon::Escape, { 256, 0, 128, 128 } } // TODO
};

ButtonDimensions* GetButtonDimensions(EButtonIcon icon)
{
    auto isPlayStation = Config::ControllerIcons == EControllerIcons::Auto
        ? hid::g_inputDeviceController == hid::EInputDevice::PlayStation
        : Config::ControllerIcons == EControllerIcons::PlayStation;

    return isPlayStation ? &g_iconsPS3[icon] : &g_iconsXenon[icon];
}

std::tuple<std::tuple<ImVec2, ImVec2>, GuestTexture*> GetButtonIcon(EButtonIcon icon)
{
    GuestTexture* btnTexture;
    auto btnTextureWidth = 256;
    auto btnTextureHeight = 128;

    switch (icon)
    {
        // TODO: replace this texture.
        case EButtonIcon::LMB:
        case EButtonIcon::Enter:
        case EButtonIcon::Escape:
            btnTexture = g_upTexKbm.get();
            btnTextureWidth = 384;
            break;

        default:
            btnTexture = g_upTexController.get();
            break;
    }

    auto btnDimensions = GetButtonDimensions(icon);
    auto btnUVs = PIXELS_TO_UV_COORDS(btnTextureWidth, btnTextureHeight, btnDimensions->X, btnDimensions->Y, btnDimensions->Width, btnDimensions->Height);

    return std::make_tuple(btnUVs, btnTexture);
}

void ButtonGuide::Init()
{
    g_upTexButtonWindow = LOAD_ZSTD_TEXTURE(g_button_window);
    g_upTexController = LOAD_ZSTD_TEXTURE(g_controller);
    g_upTexKbm = LOAD_ZSTD_TEXTURE(g_kbm);
}

void ButtonGuide::Draw()
{
    if (!s_isVisible || g_buttons.empty())
        return;

    auto* drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;

    auto fontSize = Scale(27, true);

    auto btnIconPadding = Scale(2, true);
    auto btnTextPadding = Scale(35, true);

    auto windowEdgeUVs = PIXELS_TO_UV_COORDS(64, 64, 1, 0, 40, 64);
    auto windowStretchUVs = PIXELS_TO_UV_COORDS(64, 64, 40, 0, 23, 64);
    auto windowOffsetX = g_aspectRatioOffsetX + Scale(88, true);
    auto windowOffsetY = g_aspectRatioOffsetY + Scale(114.5, true);
    auto windowEdgeWidth = Scale(40, true);
    auto windowWidth = 0.0f;
    auto windowHeight = Scale(64, true);
    auto windowColour = IM_COL32(255, 255, 255, 134);

    // Measure icons and text.
    for (int i = 0; i < g_buttons.size(); i++)
    {
        auto& btn = g_buttons[i];
    
        if (btn.Visibility && !*btn.Visibility)
            continue;
    
        auto btnWidth = Scale(GetButtonDimensions(btn.Icon)->Width, true);
        auto textWidth = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0, Localise(btn.Name).c_str()).x;
    
        windowWidth += btnWidth + textWidth + btnIconPadding + btnTextPadding;
    }

    auto windowMotionTime = g_isAnimated ? ComputeMotion(g_time, 0, 200) : 1.0;
    auto windowMotion = std::clamp(float(res.x - Scale(1088.4, true) * windowMotionTime), res.x - windowOffsetX - windowWidth, res.x - g_aspectRatioOffsetX);

    ImVec2 windowStretchMin = { windowMotion, res.y - windowOffsetY };
    ImVec2 windowStretchMax = { res.x, windowStretchMin.y + windowHeight };
    ImVec2 windowEdgeMin = { windowStretchMin.x - windowEdgeWidth, windowStretchMin.y };
    ImVec2 windowEdgeMax = { windowEdgeMin.x + windowEdgeWidth, windowEdgeMin.y + windowHeight };

    drawList->AddImage(g_upTexButtonWindow.get(), windowStretchMin, windowStretchMax, GET_UV_COORDS(windowStretchUVs), windowColour);
    drawList->AddImage(g_upTexButtonWindow.get(), windowEdgeMin, windowEdgeMax, GET_UV_COORDS(windowEdgeUVs), windowColour);

    ImVec2 windowRegionMin = { windowEdgeMax.x - Scale(8, true), windowEdgeMin.y + Scale(12.5, true) };
    ImVec2 windowRegionMax = { windowStretchMax.x, windowStretchMax.y - Scale(15, true) };

    auto btnOffsetX = 0.0f;

    // Draw icons and text.
    for (int i = 0; i < g_buttons.size(); i++)
    {
        auto& btn = g_buttons[i];

        if (btn.Visibility && !*btn.Visibility)
            continue;

        auto btnIcon = GetButtonIcon(btn.Icon);
        auto& btnUVs = std::get<0>(btnIcon);
        auto& btnTexture = std::get<1>(btnIcon);

        auto btnDimensions = GetButtonDimensions(btn.Icon);
        auto btnWidth = Scale(btnDimensions->Width, true);
        auto btnHeight = Scale(btnDimensions->Height, true);

        auto text = Localise(btn.Name).c_str();
        auto textSize = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0, text);
        auto textOffsetY = Scale(0.5, true);

        ImVec2 btnMin = { windowRegionMin.x + btnOffsetX, windowRegionMin.y };
        ImVec2 btnMax = { btnMin.x + btnWidth, btnMin.y + btnHeight };

        drawList->AddImage(btnTexture, btnMin, btnMax, GET_UV_COORDS(btnUVs));
        
        SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);
        drawList->AddText(g_pFntRodin, fontSize, { btnMax.x + btnIconPadding, btnMin.y + textOffsetY }, IM_COL32_WHITE, text);
        SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);

        btnOffsetX += btnWidth + textSize.x + btnIconPadding + btnTextPadding;
    }
}

void ButtonGuide::Open(Button button, bool isAnimated)
{
    s_isVisible = true;

    if (!g_buttons.size() || g_buttons[0] != button)
    {
        g_buttons = {};
        g_buttons.push_back(button);

        g_time = ImGui::GetTime();
        g_isAnimated = isAnimated;
    }
}

void ButtonGuide::Open(const std::span<Button> buttons, bool isAnimated)
{
    s_isVisible = true;

    auto isSameButtons = true;

    if (g_buttons.size() == buttons.size())
    {
        for (size_t i = 0; i < g_buttons.size(); i++)
        {
            if (g_buttons[i] != buttons[i])
            {
                isSameButtons = false;
                break;
            }
        }
    }
    else
    {
        isSameButtons = false;
    }

    if (!isSameButtons)
    {
        g_buttons = std::vector(buttons.begin(), buttons.end());

        g_time = ImGui::GetTime();
        g_isAnimated = isAnimated;
    }
}

void ButtonGuide::Close()
{
    s_isVisible = false;

    g_buttons.clear();
}
