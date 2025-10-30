#include "button_window.h"
#include <patches/aspect_ratio_patches.h>
#include <ui/imgui_utils.h>
#include <app.h>

static std::string g_buttonKey{};
static double g_time{};
static bool g_isAnimated{};

void ButtonWindow::Draw()
{
    if (!s_isVisible || g_buttonKey.empty())
        return;

    auto* drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;

    auto interpData = GetHidInterpTextData();
    auto fontSize = Scale(27, true);

    auto buttonLocale = &Localise(g_buttonKey);

    auto windowEdgeUVs = PIXELS_TO_UV_COORDS(64, 64, 1, 0, 40, 64);
    auto windowStretchUVs = PIXELS_TO_UV_COORDS(64, 64, 40, 0, 23, 64);
    auto windowOffsetX = g_horzCentre + Scale(128, true);
    auto windowOffsetY = g_vertCentre + Scale(114.5, true);
    auto windowEdgeWidth = Scale(40, true);
    auto windowWidth = MeasureInterpolatedText(g_pFntRodin, fontSize, buttonLocale->c_str(), &interpData).x;
    auto windowHeight = Scale(64, true);
    auto windowColour = IM_COL32(255, 255, 255, 134);

    auto windowMotionTime = g_isAnimated ? ComputeLinearMotion(g_time, 0, 100) : 1.0;
    auto windowMotion = std::clamp(float(res.x - g_horzCentre - Scale(1158.3, true) * windowMotionTime), res.x - windowOffsetX - windowWidth, res.x - g_horzCentre);

    ImVec2 windowStretchMin = { windowMotion, res.y - windowOffsetY };
    ImVec2 windowStretchMax = { res.x, windowStretchMin.y + windowHeight };
    ImVec2 windowEdgeMin = { windowStretchMin.x - windowEdgeWidth, windowStretchMin.y };
    ImVec2 windowEdgeMax = { windowEdgeMin.x + windowEdgeWidth, windowEdgeMin.y + windowHeight };

    drawList->AddImage(g_upTexButtonWindow.get(), windowStretchMin, windowStretchMax, GET_UV_COORDS(windowStretchUVs), windowColour);
    drawList->AddImage(g_upTexButtonWindow.get(), windowEdgeMin, windowEdgeMax, GET_UV_COORDS(windowEdgeUVs), windowColour);

    ImVec2 textPos = { windowEdgeMax.x - Scale(8, true), windowEdgeMin.y + Scale(12.5, true) };

    if (App::s_isInit)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);

    DrawInterpolatedText(g_pFntRodin, fontSize, textPos, IM_COL32_WHITE, buttonLocale->c_str(), &interpData);

    if (App::s_isInit)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);
}

void ButtonWindow::Open(std::string key, bool isAnimated)
{
    s_isVisible = true;

    if (g_buttonKey != key)
    {
        g_buttonKey = key;
        g_time = ImGui::GetTime();
        g_isAnimated = isAnimated;
    }
}

void ButtonWindow::Close()
{
    s_isVisible = false;
    g_buttonKey.clear();
}
