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
            auto width = (res.x - (res.y * 16.0f / 9.0f)) / 2.0f;

            drawList->AddRectFilled({ 0.0f, 0.0f }, { width, res.y }, IM_COL32_BLACK);
            drawList->AddRectFilled({ res.x - width, 0.0f }, res, IM_COL32_BLACK);
        }
        else if (WIDE_ASPECT_RATIO > g_aspectRatio)
        {
            auto height = (res.y - (res.x * 9.0f / 16.0f)) / 2.0f;

            drawList->AddRectFilled({ 0.0f, 0.0f }, { res.x, height }, IM_COL32_BLACK);
            drawList->AddRectFilled({ 0.0f, res.y - height }, res, IM_COL32_BLACK);
        }
    }

    if (!App::s_isLoading)
        g_isVisible = false;
}
