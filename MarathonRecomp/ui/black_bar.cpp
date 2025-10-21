#include "black_bar.h"
#include <patches/aspect_ratio_patches.h>
#include <ui/imgui_utils.h>
#include <app.h>

static bool g_isVisible{};
static bool g_isEdgeFade{};
static float g_margin{};

void BlackBar::Draw()
{
    if (!g_isVisible)
        return;

    auto* drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;

    auto fadeSize = Scale(50, true);

    if (g_aspectRatio > WIDE_ASPECT_RATIO)
    {
        ImVec2 leftPillarboxMin = { 0, 0 };
        ImVec2 leftPillarboxMax = { g_pillarboxWidth - g_margin, res.y };
        ImVec2 rightPillarboxMin = { (res.x - g_pillarboxWidth) + g_margin, 0 };
        ImVec2 rightPillarboxMax = { res.x, res.y };

        if (g_isEdgeFade)
            SetHorizontalGradient({ leftPillarboxMax.x - fadeSize, leftPillarboxMin.y }, leftPillarboxMax, IM_COL32_BLACK, IM_COL32_BLACK_TRANS);

        drawList->AddRectFilled(leftPillarboxMin, leftPillarboxMax, IM_COL32_BLACK);

        if (g_isEdgeFade)
            SetHorizontalGradient(rightPillarboxMin, { rightPillarboxMin.x + fadeSize, rightPillarboxMax.y }, IM_COL32_BLACK_TRANS, IM_COL32_BLACK);

        drawList->AddRectFilled(rightPillarboxMin, rightPillarboxMax, IM_COL32_BLACK);
    }
    else if (WIDE_ASPECT_RATIO > g_aspectRatio)
    {
        ImVec2 topLetterboxMin = { 0, 0 };
        ImVec2 topLetterboxMax = { res.x, g_letterboxHeight - g_margin };
        ImVec2 bottomLetterboxMin = { 0, res.y - g_letterboxHeight + g_margin };
        ImVec2 bottomLetterboxMax = { res.x, res.y };

        if (g_isEdgeFade)
            SetVerticalGradient({ topLetterboxMin.x, topLetterboxMax.y - fadeSize }, topLetterboxMax, IM_COL32_BLACK, IM_COL32_BLACK_TRANS);

        drawList->AddRectFilled(topLetterboxMin, topLetterboxMax, IM_COL32_BLACK);

        if (g_isEdgeFade)
            SetVerticalGradient(bottomLetterboxMin, { bottomLetterboxMax.x, bottomLetterboxMin.y + fadeSize }, IM_COL32_BLACK_TRANS, IM_COL32_BLACK);

        drawList->AddRectFilled(bottomLetterboxMin, bottomLetterboxMax, IM_COL32_BLACK);
    }

    if (g_isEdgeFade)
        ResetGradient();

    if (App::s_isLoading)
        return;

    g_isVisible = false;
    g_isEdgeFade = false;
    g_margin = 0.0f;
}

void BlackBar::Show(bool isEdgeFade)
{
    g_isVisible = true;
    g_isEdgeFade = isEdgeFade;
}

void BlackBar::Hide()
{
    g_isVisible = false;
}

void BlackBar::SetBorderMargin(float margin)
{
    g_margin = margin;
}

bool BlackBar::IsVisible()
{
    return g_isVisible;
}
