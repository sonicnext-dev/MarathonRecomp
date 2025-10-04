#include "options_menu.h"
#include <locale/locale.h>
#include <patches/aspect_ratio_patches.h>
#include <ui/common_menu.h>
#include <ui/imgui_utils.h>
#include <user/config.h>

static CommonMenu g_commonMenu;
static double g_appearTime = 0.0;

void OptionsMenu::Draw()
{
    if (!s_isVisible)
        return;

    switch (s_state)
    {
        case OptionsMenuState::Opening:
            g_commonMenu.Open();
            s_state = OptionsMenuState::Idle;
            break;

        default:
        {
            auto* drawList = ImGui::GetBackgroundDrawList();
            auto& res = ImGui::GetIO().DisplaySize;

            auto alphaMotionTime = s_isPause ? ComputeMotion(g_appearTime, 0, 10) : 0.0;
            auto alpha = s_isPause ? Lerp(0, 175, alphaMotionTime) : 255;
            auto gradientTop = IM_COL32(0, 103, 255, alpha);
            auto gradientBottom = IM_COL32(0, 41, 100, alpha);

            drawList->AddRectFilledMultiColor({ 0.0f, g_letterboxHeight }, { res.x, res.y - g_letterboxHeight }, gradientTop, gradientTop, gradientBottom, gradientBottom);

            DrawArrows({ 0, res.y / 2 - Scale(10) }, res);
            g_commonMenu.Draw();

            break;
        }
    }
}

void OptionsMenu::Open(bool isPause)
{
    g_commonMenu = CommonMenu(Localise("Options_Header_Name").c_str(), nullptr, isPause);
    g_appearTime = ImGui::GetTime();

    s_state = OptionsMenuState::Opening;
    s_isVisible = true;
    s_isPause = isPause;
}

void OptionsMenu::Close()
{
    if (s_state != OptionsMenuState::Closing)
    {
        g_appearTime = ImGui::GetTime();
        s_state = OptionsMenuState::Closing;

        Config::Save();
    }

    s_isVisible = false;
}

bool OptionsMenu::CanClose()
{
    return true;
}
