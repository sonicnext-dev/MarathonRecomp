#include "black_bar.h"
#include <patches/aspect_ratio_patches.h>
#include <app.h>

void BlackBar::Draw()
{
    if (g_isVisible)
    {
        auto drawList = ImGui::GetBackgroundDrawList();
        auto& res = ImGui::GetIO().DisplaySize;

        if (g_aspectRatio > WIDE_ASPECT_RATIO)
        {
            drawList->AddRectFilled({ 0.0f, 0.0f }, { g_aspectRatioOffsetX, res.y }, IM_COL32_BLACK);
            drawList->AddRectFilled({ res.x - g_aspectRatioOffsetX, 0.0f }, res, IM_COL32_BLACK);
        }
        else if (WIDE_ASPECT_RATIO > g_aspectRatio)
        {
            drawList->AddRectFilled({ 0.0f, 0.0f }, { res.x, g_aspectRatioOffsetY }, IM_COL32_BLACK);
            drawList->AddRectFilled({ 0.0f, res.y - g_aspectRatioOffsetY }, res, IM_COL32_BLACK);
        }
    }

    if (!App::s_isLoading)
        g_isVisible = false;
}
