#include "achievement_overlay.h"
#include <kernel/memory.h>
#include <kernel/xdbf.h>
#include <locale/achievement_locale.h>
#include <locale/locale.h>
#include <ui/imgui_utils.h>
#include <user/achievement_data.h>
#include <user/config.h>
#include <app.h>
#include <exports.h>

constexpr double OVERLAY_DURATION = 5.0;

static bool g_isClosing{};
static double g_appearTime{};

static Achievement g_achievement{};

// Dequeue achievements only in the main thread. This is also extra thread safety.
static std::thread::id g_mainThreadId = std::this_thread::get_id();

static bool CanDequeueAchievement()
{
    return std::this_thread::get_id() == g_mainThreadId && !AchievementOverlay::s_queue.empty();
}

void AchievementOverlay::Draw()
{
    if (!AchievementOverlay::s_isVisible && CanDequeueAchievement())
    {
        s_isVisible = true;
        g_isClosing = false;
        g_appearTime = ImGui::GetTime();
        g_achievement = g_xdbfWrapper.GetAchievement((EXDBFLanguage)Config::Language.Value, s_queue.front());
        s_queue.pop();

        Game_PlaySound("deside");
    }
    
    if (!s_isVisible)
        return;
    
    if (ImGui::GetTime() - g_appearTime >= OVERLAY_DURATION)
        AchievementOverlay::Close();

    auto drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;

    auto& strAchievementUnlocked = Localise("Achievements_Unlock");
    auto& strAchievementName = g_achievement.Name;

    if (!Config::UseOfficialAchievementText)
        strAchievementName = GetAchievementLocale(g_achievement.ID).Name;

    // Calculate text sizes.
    auto fontSize = Scale(Config::Language == ELanguage::Japanese ? 28 : 27, true);
    auto headerSize = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0, strAchievementUnlocked.c_str());
    auto nameSize = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0, strAchievementName.c_str());
    auto nameOffsetY = Scale(6);
    auto maxWidth = std::max(headerSize.x, nameSize.x) + Scale(5);
    auto maxHeight = headerSize.y + nameSize.y + nameOffsetY;

    // Calculate margins.
    auto imageMarginX = Scale(25);
    auto imageMarginY = Scale(20);
    auto imageSize = Scale(64);
    auto textMarginX = imageMarginX * 2 + imageSize - Scale(5);
    auto containerWidth = imageMarginX + textMarginX + maxWidth;
    auto containerHeight = Scale(105);

    ImVec2 min = { (res.x / 2) - (containerWidth / 2), Scale(55) };
    ImVec2 max = { min.x + containerWidth, min.y + containerHeight };

    // Calculate centred vertical text offset.
    auto textMarginY = (min.y + (containerHeight / 2)) - (maxHeight / 2);

    auto windowMotion = DrawWindow(min, max, true, g_appearTime, g_isClosing);

    if (windowMotion >= 1.0)
    {
        // Draw achievement icon.
        drawList->AddImage
        (
            g_xdbfTextureCache[g_achievement.ID],
            { /* X */ min.x + imageMarginX, /* Y */ min.y + imageMarginY },
            { /* X */ min.x + imageMarginX + imageSize, /* Y */ min.y + imageMarginY + imageSize },
            { 0, 0 },
            { 1, 1 },
            IM_COL32(255, 255, 255, 255)
        );

        SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);

        auto colourMotion = BREATHE_MOTION(1.0f, 0.0f, g_appearTime, 0.9f);
        auto headerColour = ColourLerp(IM_COL32_WHITE, IM_COL32(255, 153, 0, 255), colourMotion);

        // Draw header text.
        drawList->AddText
        (
            g_pFntRodin,
            fontSize,
            { /* X */ min.x + textMarginX + (maxWidth - headerSize.x) / 2, /* Y */ textMarginY },
            headerColour,
            strAchievementUnlocked.c_str()
        );

        // Draw achievement name.
        drawList->AddText
        (
            g_pFntRodin,
            fontSize,
            { /* X */ min.x + textMarginX + (maxWidth - nameSize.x) / 2, /* Y */ textMarginY + nameSize.y + nameOffsetY },
            IM_COL32(255, 255, 255, 255),
            strAchievementName.c_str()
        );

        SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);
    }
    else if (windowMotion <= 0.0 && g_isClosing)
    {
        s_isVisible = false;
    }

    drawList->PopClipRect();
}

void AchievementOverlay::Open(int id)
{
    s_queue.push(id);
}

void AchievementOverlay::Close()
{
    if (!g_isClosing)
    {
        g_appearTime = ImGui::GetTime();
        g_isClosing = true;
    }

    if (CanDequeueAchievement())
        s_isVisible = false;
}
