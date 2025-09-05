#include "black_bar.h"
#include <patches/aspect_ratio_patches.h>
#include <app.h>

void BlackBar::Draw()
{
    if (!g_isPillarbox)
        return;

    auto drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;

    float width = (res.x - (res.y * 16.0f / 9.0f)) / 2.0f;

    drawList->AddRectFilled(
        { 0.0f, 0.0f },
        { width, res.y },
        IM_COL32_BLACK);

    drawList->AddRectFilled(
        { res.x - width, 0.0f },
        res,
        IM_COL32_BLACK);

    if (!App::s_isLoading)
        g_isPillarbox = false;
}
