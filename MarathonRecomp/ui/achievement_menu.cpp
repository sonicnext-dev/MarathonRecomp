#include "achievement_menu.h"
#include <api/Marathon.h>
#include <gpu/imgui/imgui_snapshot.h>
#include <gpu/video.h>
#include <hid/hid.h>
#include <kernel/xdbf.h>
#include <locale/locale.h>
#include <patches/aspect_ratio_patches.h>
#include <ui/button_guide.h>
#include <ui/imgui_utils.h>
#include <user/achievement_manager.h>
#include <user/config.h>
#include <app.h>
#include <exports.h>
#include <decompressor.h>

#include <res/images/achievements_menu/trophy.dds.h>

constexpr double HEADER_CONTAINER_INTRO_MOTION_START = 0;
constexpr double HEADER_CONTAINER_INTRO_MOTION_END = 15;
constexpr double HEADER_CONTAINER_OUTRO_MOTION_START = 0;
constexpr double HEADER_CONTAINER_OUTRO_MOTION_END = 40;
constexpr double HEADER_CONTAINER_INTRO_FADE_START = 5;
constexpr double HEADER_CONTAINER_INTRO_FADE_END = 14;
constexpr double HEADER_CONTAINER_OUTRO_FADE_START = 0;
constexpr double HEADER_CONTAINER_OUTRO_FADE_END = 7;

constexpr double CONTENT_CONTAINER_COMMON_MOTION_START = 11;
constexpr double CONTENT_CONTAINER_COMMON_MOTION_END = 12;

constexpr double COUNTER_INTRO_FADE_START = 15;
constexpr double COUNTER_INTRO_FADE_END = 16;

constexpr double SELECTION_CONTAINER_BREATHE = 30;

static bool g_isClosing = false;

static double g_appearTime;

static std::vector<std::tuple<Achievement, time_t>> g_achievements;

static ImFont* g_rodinFont;
static ImFont* g_newRodinFont;

static std::unique_ptr<GuestTexture> g_upTrophyIcon;

static int g_firstVisibleRowIndex;
static int g_selectedRowIndex;
static double g_rowSelectionTime;
static double g_lastTappedTime;
static double g_lastIncrementTime;

static bool g_upWasHeld;
static bool g_downWasHeld;

static void ResetSelection()
{
    g_firstVisibleRowIndex = 0;
    g_selectedRowIndex = 0;
    g_rowSelectionTime = ImGui::GetTime();
    g_upWasHeld = false;
    g_downWasHeld = false;
}

static void DrawContainer(ImVec2 min, ImVec2 max, ImU32 gradientTop, ImU32 gradientBottom, float alpha = 1, float cornerRadius = 25)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    // DrawPauseContainer(min, max, alpha);

    drawList->PushClipRect({ min.x, min.y + Scale(20) }, { max.x, max.y - Scale(5) });
}

static void DrawHeaderContainer(const char* text)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    auto fontSize = Scale(24);
    auto minTextSize = Scale(294.575989);
    auto textSize = g_newRodinFont->CalcTextSizeA(fontSize, FLT_MAX, 0, text);
    auto cornerRadius = 23;
    auto textMarginX = Scale(16) + (Scale(cornerRadius) / 2);

    auto containerMotion = g_isClosing
        ? ComputeMotion(g_appearTime, HEADER_CONTAINER_OUTRO_MOTION_START, HEADER_CONTAINER_OUTRO_MOTION_END)
        : ComputeMotion(g_appearTime, HEADER_CONTAINER_INTRO_MOTION_START, HEADER_CONTAINER_INTRO_MOTION_END);

    auto colourMotion = g_isClosing
        ? ComputeMotion(g_appearTime, HEADER_CONTAINER_OUTRO_FADE_START, HEADER_CONTAINER_OUTRO_FADE_END)
        : ComputeMotion(g_appearTime, HEADER_CONTAINER_INTRO_FADE_START, HEADER_CONTAINER_INTRO_FADE_END);

    // Slide animation.
    auto containerMarginX = g_isClosing
        ? Hermite(251, 151, containerMotion)
        : Hermite(151, 251, containerMotion);

    // Transparency fade animation.
    auto alpha = g_isClosing
        ? Lerp(1, 0, colourMotion)
        : Lerp(0, 1, colourMotion);

    ImVec2 min = { g_aspectRatioOffsetX + Scale(containerMarginX), g_aspectRatioOffsetY + Scale(136) };
    ImVec2 max = { std::max(min.x + minTextSize, min.x + textMarginX * 2 + textSize.x + Scale(5)), g_aspectRatioOffsetY + Scale(196) };

    // DrawPauseHeaderContainer(min, max, alpha);

    SetTextSkew((min.y + max.y) / 2.0f, Scale(3.0f));

    DrawTextWithOutline
    (
        g_newRodinFont,
        fontSize,
        { /* X */ min.x + textMarginX, /* Y */ CENTRE_TEXT_VERT(min, max, textSize) - Scale(5) },
        IM_COL32(255, 255, 255, 255 * alpha),
        text,
        4,
        IM_COL32(0, 0, 0, 255 * alpha)
    );

    ResetTextSkew();
}

static void DrawAchievement(int rowIndex, float yOffset, Achievement& achievement, bool isUnlocked)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    auto clipRectMin = drawList->GetClipRectMin();
    auto clipRectMax = drawList->GetClipRectMax();

    auto itemWidth = Scale(700);
    auto itemHeight = Scale(94);
    auto itemMarginX = Scale(18);
    auto imageMarginX = Scale(25);
    auto imageMarginY = Scale(18);
    auto imageSize = Scale(60);

    ImVec2 min = { itemMarginX + clipRectMin.x, clipRectMin.y + itemHeight * rowIndex + yOffset };
    ImVec2 max = { itemMarginX + min.x + itemWidth, min.y + itemHeight };

    auto icon = g_xdbfTextureCache[achievement.ID];
    auto isSelected = rowIndex == g_selectedRowIndex;

    // if (isSelected)
    //     DrawSelectionContainer(min, max);

    auto desc = isUnlocked ? achievement.UnlockedDesc.c_str() : achievement.LockedDesc.c_str();
    auto fontSize = Scale(24);
    auto textSize = g_rodinFont->CalcTextSizeA(fontSize, FLT_MAX, 0, desc);
    auto textX = min.x + imageMarginX + imageSize + itemMarginX * 2;
    auto textMarqueeX = min.x + imageMarginX + imageSize;
    auto titleTextY = Scale(20);
    auto descTextY = Scale(52);

    if (!isUnlocked)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_GRAYSCALE);

    // Draw achievement icon.
    drawList->AddImage
    (
        icon,
        { /* X */ min.x + imageMarginX, /* Y */ min.y + imageMarginY },
        { /* X */ min.x + imageMarginX + imageSize, /* Y */ min.y + imageMarginY + imageSize },
        { /* U */ 0, /* V */ 0 },
        { /* U */ 1, /* V */ 1 },
        IM_COL32(255, 255, 255, 255 * (isUnlocked ? 1 : 0.5f))
    );

    if (!isUnlocked)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);

    drawList->PushClipRect(min, max, true);

    auto colLockedText = IM_COL32(80, 80, 80, 127);

    auto colTextShadow = isUnlocked
        ? IM_COL32(0, 0, 0, 255)
        : IM_COL32(20, 20, 20, 127);

    auto shadowOffset = isUnlocked ? 2 : 1;
    auto shadowRadius = isUnlocked ? 1 : 0.5f;

    // Draw achievement name.
    DrawTextWithShadow
    (
        g_rodinFont,
        fontSize,
        { textX, min.y + titleTextY },
        isUnlocked ? IM_COL32(252, 243, 5, 255) : colLockedText,
        achievement.Name.c_str(),
        shadowOffset,
        shadowRadius,
        colTextShadow
    );

    ImVec2 marqueeMin = { textMarqueeX, min.y };
    ImVec2 marqueeMax = { max.x - Scale(10) /* timestamp margin X */, max.y };

    SetHorizontalMarqueeFade(marqueeMin, marqueeMax, Scale(32));

    if (isSelected && textX + textSize.x >= max.x - Scale(10))
    {
        // Draw achievement description with marquee.
        DrawTextWithMarqueeShadow
        (
            g_rodinFont,
            fontSize,
            { textX, min.y + descTextY },
            marqueeMin,
            marqueeMax,
            isUnlocked ? IM_COL32_WHITE : colLockedText,
            desc,
            g_rowSelectionTime,
            0.9,
            Scale(250),
            shadowOffset,
            shadowRadius,
            colTextShadow
        );
    }
    else
    {
        // Draw achievement description.
        DrawTextWithShadow
        (
            g_rodinFont,
            fontSize,
            { textX, min.y + descTextY },
            isUnlocked ? IM_COL32_WHITE : colLockedText,
            desc,
            shadowOffset,
            shadowRadius,
            colTextShadow
        );
    }

    ResetMarqueeFade();

    drawList->PopClipRect();

    if (!isUnlocked)
        return;

    auto timestamp = AchievementManager::GetTimestamp(achievement.ID);

    if (!timestamp)
        return;

    char buffer[32];
#ifdef _WIN32
    tm timeStruct;
    tm *timePtr = &timeStruct;
    localtime_s(timePtr, &timestamp);
#else
    tm *timePtr = localtime(&timestamp);
#endif
    strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M", timePtr);

    fontSize = Scale(12);
    textSize = g_newRodinFont->CalcTextSizeA(fontSize, FLT_MAX, 0, buffer);

    auto containerMarginX = Scale(10);
    auto textMarginX = Scale(8);

    ImVec2 timestampMin = { max.x - containerMarginX - textSize.x - (textMarginX * 2), min.y + titleTextY };
    ImVec2 timestampMax = { max.x - containerMarginX, min.y + Scale(46) };

    drawList->PushClipRect(min, max, true);

    auto bevelOffset = Scale(6);

    // Left
    drawList->AddRectFilledMultiColor
    (
        timestampMin,
        { timestampMin.x + bevelOffset, timestampMax.y },
        IM_COL32(255, 255, 255, 255),
        IM_COL32(149, 149, 149, 40),
        IM_COL32(149, 149, 149, 40),
        IM_COL32(255, 255, 255, 255)
    );

    // Right
    drawList->AddRectFilledMultiColor
    (
        { timestampMax.x - bevelOffset, timestampMin.y },
        { timestampMax.x, timestampMax.y },
        IM_COL32(149, 149, 149, 40),
        IM_COL32(255, 255, 255, 255),
        IM_COL32(255, 255, 255, 255),
        IM_COL32(149, 149, 149, 40)
    );

    // Centre
    drawList->AddRectFilled
    (
        { timestampMin.x, timestampMin.y + bevelOffset },
        { timestampMax.x, timestampMax.y - bevelOffset },
        IM_COL32(38, 38, 38, 172)
    );

    // Top
    drawList->AddRectFilledMultiColor
    (
        timestampMin,
        { timestampMax.x, timestampMin.y + bevelOffset },
        IM_COL32(16, 16, 16, 192),
        IM_COL32(16, 16, 16, 192),
        IM_COL32(38, 38, 38, 172),
        IM_COL32(38, 38, 38, 172)
    );

    // Bottom
    drawList->AddRectFilledMultiColor
    (
        { timestampMin.x, timestampMax.y - bevelOffset },
        { timestampMax.x, timestampMax.y },
        IM_COL32(38, 40, 38, 169),
        IM_COL32(38, 40, 38, 169),
        IM_COL32(16, 16, 16, 192),
        IM_COL32(16, 16, 16, 192)
    );

    // Draw timestamp text.
    DrawTextWithOutline
    (
        g_newRodinFont,
        fontSize,
        { /* X */ CENTRE_TEXT_HORZ(timestampMin, timestampMax, textSize), /* Y */ CENTRE_TEXT_VERT(timestampMin, timestampMax, textSize) },
        IM_COL32(255, 255, 255, 255),
        buffer,
        4,
        IM_COL32(8, 8, 8, 255)
    );

    drawList->PopClipRect();
}

static void DrawTrophySparkles(ImVec2 min, ImVec2 max, int recordCount, int trophyFrameIndex)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    constexpr auto recordsHalfTotal = ACH_RECORDS / 2;

    // Don't sparkle the bronze trophy.
    if (recordCount < recordsHalfTotal)
        return;

    static int trophyAnimCycles = 0;
    static bool isIncrementedCycles = false;

    bool isGoldTrophy = recordCount >= ACH_RECORDS;

    if (!trophyFrameIndex && !isIncrementedCycles)
    {
        trophyAnimCycles++;
        trophyAnimCycles %= isGoldTrophy ? 4 : 3;
        isIncrementedCycles = true;
    }

    if (trophyFrameIndex >= 1)
        isIncrementedCycles = false;

    if (trophyAnimCycles >= 2)
    {
        auto marginX = Scale(9);
        auto uv = PIXELS_TO_UV_COORDS(2048, 1024, 1984, 960, 64, 64);
        auto& uv0 = std::get<0>(uv);
        auto& uv1 = std::get<1>(uv);
        auto colour = IM_COL32(240, 240, 200, 200);

        static auto scaleStart = ImGui::GetTime();
        auto scale = Scale(18) * Hermite(1.0f, 0.0f, (sin((ImGui::GetTime() - scaleStart) * (2.0f * M_PI / (15.0f / 60.0f))) + 1.0f) / 2.0f);

        // Don't do extra sparkles for the silver trophy.
        if (isGoldTrophy)
        {
            if (trophyFrameIndex >= 0 && trophyFrameIndex <= 5)
            {
                auto marginXAdd = Scale(1);

                // Centre Left
                drawList->AddImage
                (
                    g_upTrophyIcon.get(),
                    { min.x - scale / 2 + marginX + marginXAdd, max.y - ((max.y - min.y) / 2) - scale / 2 },
                    { min.x + scale / 2 + marginX + marginXAdd, max.y - ((max.y - min.y) / 2) + scale / 2 },
                    uv0, uv1,
                    colour
                );
            }

            if (trophyFrameIndex >= 16 && trophyFrameIndex <= 21)
            {
                auto marginXAdd = Scale(4);
                auto marginY = Scale(11);

                // Bottom Right
                drawList->AddImage
                (
                    g_upTrophyIcon.get(),
                    { max.x - scale / 2 - (marginX + marginXAdd), max.y - scale / 2 - marginY },
                    { max.x + scale / 2 - (marginX + marginXAdd), max.y + scale / 2 - marginY },
                    uv0, uv1,
                    colour
                );
            }
        }
        
        if (trophyFrameIndex >= 24 && trophyFrameIndex <= 29)
        {
            auto marginY = Scale(1);

            // Top Right
            drawList->AddImage
            (
                g_upTrophyIcon.get(),
                { max.x - scale / 2 - marginX, min.y - scale / 2 + marginY },
                { max.x + scale / 2 - marginX, min.y + scale / 2 + marginY },
                uv0, uv1,
                colour
            );
        }
    }
}

static void DrawAchievementTotal(ImVec2 min, ImVec2 max)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    // Transparency fade animation.
    auto alpha = Cubic(0, 1, ComputeMotion(g_appearTime, COUNTER_INTRO_FADE_START, COUNTER_INTRO_FADE_END));

    auto imageMarginX = Scale(5);
    auto imageMarginY = Scale(5);
    auto imageSize = Scale(45);

    ImVec2 imageMin = { max.x - imageSize - imageMarginX, min.y - imageSize - imageMarginY };
    ImVec2 imageMax = { imageMin.x + imageSize, imageMin.y + imageSize };

    constexpr auto columns = 8;
    constexpr auto rows = 4;
    constexpr auto spriteSize = 256.0f;
    constexpr auto textureWidth = 2048.0f;
    constexpr auto textureHeight = 1024.0f;
    auto frameIndex = int32_t(floor(ImGui::GetTime() * 30.0f)) % 30;
    auto columnIndex = frameIndex % columns;
    auto rowIndex = frameIndex / columns;
    auto uv0 = ImVec2(columnIndex * spriteSize / textureWidth, rowIndex * spriteSize / textureHeight);
    auto uv1 = ImVec2((columnIndex + 1) * spriteSize / textureWidth, (rowIndex + 1) * spriteSize / textureHeight);

    constexpr auto recordsHalfTotal = ACH_RECORDS / 2;
    auto records = AchievementManager::GetTotalRecords();

    ImVec4 colBronze = ImGui::ColorConvertU32ToFloat4(IM_COL32(198, 105, 15, 255 * alpha));
    ImVec4 colSilver = ImGui::ColorConvertU32ToFloat4(IM_COL32(220, 220, 220, 255 * alpha));
    ImVec4 colGold   = ImGui::ColorConvertU32ToFloat4(IM_COL32(255, 195, 56, 255 * alpha));
    ImVec4 colResult;

    if (records <= 25)
    {
        float t = (float)records / 25.0f;

        // Fade from bronze to silver.
        colResult.x = colBronze.x + t * (colSilver.x - colBronze.x);
        colResult.y = colBronze.y + t * (colSilver.y - colBronze.y);
        colResult.z = colBronze.z + t * (colSilver.z - colBronze.z);
        colResult.w = colBronze.w + t * (colSilver.w - colBronze.w);
    }
    else if (records <= 50)
    {
        float t = ((float)records - 25.0f) / 25.0f;

        // Fade from silver to gold.
        colResult.x = colSilver.x + t * (colGold.x - colSilver.x);
        colResult.y = colSilver.y + t * (colGold.y - colSilver.y);
        colResult.z = colSilver.z + t * (colGold.z - colSilver.z);
        colResult.w = colSilver.w + t * (colGold.w - colSilver.w);
    }
    else
    {
        colResult = colGold;
    }

    drawList->AddImage(g_upTrophyIcon.get(), imageMin, imageMax, uv0, uv1, ImGui::ColorConvertFloat4ToU32(colResult));

    // Add extra luminance to the trophy for bronze and gold.
    if (records < recordsHalfTotal || records >= ACH_RECORDS)
        drawList->AddImage(g_upTrophyIcon.get(), imageMin, imageMax, uv0, uv1, IM_COL32(255, 255, 255, 12));

    // Draw sparkles on the trophy for silver and gold.
    if (records >= recordsHalfTotal || records >= ACH_RECORDS)
        DrawTrophySparkles(imageMin, imageMax, records, frameIndex);

    auto str = fmt::format("{} / {}", records, ACH_RECORDS);
    auto fontSize = Scale(20);
    auto textSize = g_newRodinFont->CalcTextSizeA(fontSize, FLT_MAX, 0, str.c_str());

    DrawTextWithOutline
    (
        g_newRodinFont,
        fontSize,
        { /* X */ imageMin.x - textSize.x - Scale(6), /* Y */ CENTRE_TEXT_VERT(imageMin, imageMax, textSize) },
        IM_COL32(255, 255, 255, 255 * alpha),
        str.c_str(),
        4,
        IM_COL32(0, 0, 0, 255 * alpha)
    );
}

//static void DrawContentContainer()
//{
//    auto drawList = ImGui::GetBackgroundDrawList();
//
//    // Expand/retract animation.
//    auto motion = g_isClosing
//        ? ComputeMotion(g_appearTime, 0, CONTENT_CONTAINER_COMMON_MOTION_START)
//        : ComputeMotion(g_appearTime, CONTENT_CONTAINER_COMMON_MOTION_START, CONTENT_CONTAINER_COMMON_MOTION_END);
//
//    auto minX = g_isClosing
//        ? Hermite(251, 301, motion)
//        : Hermite(301, 251, motion);
//
//    auto minY = g_isClosing
//        ? Hermite(189, 206, motion)
//        : Hermite(206, 189, motion);
//
//    auto maxX = g_isClosing
//        ? Hermite(1031, 978, motion)
//        : Hermite(978, 1031, motion);
//
//    auto maxY = g_isClosing
//        ? Hermite(604, 573, motion)
//        : Hermite(573, 604, motion);
//
//    ImVec2 min = { g_aspectRatioOffsetX + Scale(minX), g_aspectRatioOffsetY + Scale(minY) };
//    ImVec2 max = { g_aspectRatioOffsetX + Scale(maxX), g_aspectRatioOffsetY + Scale(maxY) };
//
//    // Transparency fade animation.
//    auto alpha = g_isClosing
//        ? Hermite(1, 0, motion)
//        : Hermite(0, 1, motion);
//
//    DrawContainer(min, max, IM_COL32(197, 194, 197, 200), IM_COL32(115, 113, 115, 236), alpha);
//
//    if (motion < 1.0f)
//    {
//        drawList->PopClipRect();
//        return;
//    }
//    else if (g_isClosing)
//    {
//        AchievementMenu::s_isVisible = false;
//        drawList->PopClipRect();
//        return;
//    }
//
//    auto clipRectMin = drawList->GetClipRectMin();
//    auto clipRectMax = drawList->GetClipRectMax();
//
//    auto itemHeight = Scale(94);
//    auto yOffset = -g_firstVisibleRowIndex * itemHeight + Scale(2);
//    auto rowCount = 0;
//
//    // Draw separators.
//    for (int i = 1; i <= 3; i++)
//    {
//        ImVec2 lineMin = { clipRectMin.x + Scale(35), clipRectMin.y + itemHeight * i + Scale(2) };
//        ImVec2 lineMax = { clipRectMax.x - Scale(55), lineMin.y + Scale(1.3f) };
//
//        SetAdditive(true);
//        drawList->AddRectFilled(lineMin, lineMax, IM_COL32(160, 160, 160, 60));
//        SetAdditive(false);
//    }
//
//    for (auto& tpl : g_achievements)
//    {
//        auto achievement = std::get<0>(tpl);
//
//        if (AchievementManager::IsUnlocked(achievement.ID))
//            DrawAchievement(rowCount++, yOffset, achievement, true);
//    }
//
//    for (auto& tpl : g_achievements)
//    {
//        auto achievement = std::get<0>(tpl);
//
//        if (!AchievementManager::IsUnlocked(achievement.ID))
//            DrawAchievement(rowCount++, yOffset, achievement, false);
//    }
//
//    auto inputState = SWA::CInputState::GetInstance();
//
//    bool upIsHeld = inputState->GetPadState().IsDown(SWA::eKeyState_DpadUp) ||
//        inputState->GetPadState().LeftStickVertical > 0.5f;
//
//    bool downIsHeld = inputState->GetPadState().IsDown(SWA::eKeyState_DpadDown) ||
//        inputState->GetPadState().LeftStickVertical < -0.5f;
//
//    bool isReachedTop = g_selectedRowIndex == 0;
//    bool isReachedBottom = g_selectedRowIndex == rowCount - 1;
//
//    bool scrollUp = !g_upWasHeld && upIsHeld;
//    bool scrollDown = !g_downWasHeld && downIsHeld;
//
//    auto time = ImGui::GetTime();
//    auto fastScroll = (time - g_lastTappedTime) > 0.6;
//    auto fastScrollSpeed = 1.0 / 3.5;
//    static auto fastScrollSpeedUp = false;
//
//    if (scrollUp || scrollDown)
//        g_lastTappedTime = time;
//
//    if (!upIsHeld && !downIsHeld)
//        fastScrollSpeedUp = false;
//
//    if (fastScrollSpeedUp)
//        fastScrollSpeed /= 2;
//
//    if (fastScroll)
//    {
//        if ((time - g_lastIncrementTime) < fastScrollSpeed)
//        {
//            fastScroll = false;
//        }
//        else
//        {
//            g_lastIncrementTime = time;
//
//            scrollUp = upIsHeld;
//            scrollDown = downIsHeld;
//            fastScrollSpeedUp = true;
//        }
//    }
//
//    if (scrollUp)
//    {
//        --g_selectedRowIndex;
//        if (g_selectedRowIndex < 0)
//            g_selectedRowIndex = rowCount - 1;
//    }
//    else if (scrollDown)
//    {
//        ++g_selectedRowIndex;
//        if (g_selectedRowIndex >= rowCount)
//            g_selectedRowIndex = 0;
//    }
//
//    if (scrollUp || scrollDown)
//    {
//        g_rowSelectionTime = time;
//        Game_PlaySound("sys_actstg_pausecursor");
//    }
//
//    g_upWasHeld = upIsHeld;
//    g_downWasHeld = downIsHeld;
//
//    int visibleRowCount = int(floor((clipRectMax.y - clipRectMin.y) / itemHeight));
//
//    if (g_firstVisibleRowIndex > g_selectedRowIndex)
//        g_firstVisibleRowIndex = g_selectedRowIndex;
//
//    if (g_firstVisibleRowIndex + visibleRowCount - 1 < g_selectedRowIndex)
//        g_firstVisibleRowIndex = std::max(0, g_selectedRowIndex - visibleRowCount + 1);
//
//    // Pop clip rect from DrawContentContainer
//    drawList->PopClipRect();
//
//    DrawAchievementTotal(min, max);
//
//    // Draw scroll bar
//    if (rowCount > visibleRowCount)
//    {
//        float cornerRadius = Scale(25);
//        float totalHeight = (clipRectMax.y - clipRectMin.y - cornerRadius) - Scale(5);
//        float heightRatio = float(visibleRowCount) / float(rowCount);
//        float offsetRatio = float(g_firstVisibleRowIndex) / float(rowCount);
//        float offsetX = clipRectMax.x - Scale(39);
//        float offsetY = offsetRatio * totalHeight + clipRectMin.y + Scale(4);
//        float maxY = max.y - cornerRadius - Scale(3);
//        float lineThickness = Scale(1);
//        float innerMarginX = Scale(2);
//        float outerMarginX = Scale(24);
//
//        // Outline
//        drawList->AddRect
//        (
//            { /* X */ offsetX - lineThickness, /* Y */ clipRectMin.y - lineThickness },
//            { /* X */ clipRectMax.x - outerMarginX + lineThickness, /* Y */ maxY + lineThickness },
//            IM_COL32(255, 255, 255, 155),
//            Scale(1)
//        );
//
//        // Background
//        drawList->AddRectFilledMultiColor
//        (
//            { /* X */ offsetX, /* Y */ clipRectMin.y },
//            { /* X */ clipRectMax.x - outerMarginX, /* Y */ maxY },
//            IM_COL32(123, 125, 123, 255),
//            IM_COL32(123, 125, 123, 255),
//            IM_COL32(97, 99, 97, 255),
//            IM_COL32(97, 99, 97, 255)
//        );
//
//        // Scroll Bar Outline
//        drawList->AddRectFilledMultiColor
//        (
//            { /* X */ offsetX + innerMarginX, /* Y */ offsetY - lineThickness },
//            { /* X */ clipRectMax.x - outerMarginX - innerMarginX, /* Y */ offsetY + lineThickness + totalHeight * heightRatio },
//            IM_COL32(185, 185, 185, 255),
//            IM_COL32(185, 185, 185, 255),
//            IM_COL32(172, 172, 172, 255),
//            IM_COL32(172, 172, 172, 255)
//        );
//
//        // Scroll Bar
//        drawList->AddRectFilled
//        (
//            { /* X */ offsetX + innerMarginX + lineThickness, /* Y */ offsetY },
//            { /* X */ clipRectMax.x - outerMarginX - innerMarginX - lineThickness, /* Y */ offsetY + totalHeight * heightRatio },
//            IM_COL32(255, 255, 255, 255)
//        );
//    }
//}

void AchievementMenu::Init()
{
    auto& io = ImGui::GetIO();

    g_rodinFont = ImFontAtlasSnapshot::GetFont("FOT-RodinPro-DB.otf");
    g_newRodinFont = ImFontAtlasSnapshot::GetFont("FOT-NewRodinPro-UB.otf");

    g_upTrophyIcon = LOAD_ZSTD_TEXTURE(g_trophy);
}

void AchievementMenu::Draw()
{
    if (!s_isVisible)
        return;

    DrawHeaderContainer(Localise("Achievements_Name_Uppercase").c_str());
//    DrawContentContainer();
}

void AchievementMenu::Open()
{
    s_isVisible = true;
    g_isClosing = false;
    g_appearTime = ImGui::GetTime();

    g_achievements.clear();

    for (auto& achievement : g_xdbfWrapper.GetAchievements((EXDBFLanguage)Config::Language.Value))
    {
        if (Config::Language == ELanguage::English)
            achievement.Name = xdbf::FixInvalidSequences(achievement.Name);

        g_achievements.push_back(std::make_tuple(achievement, AchievementManager::GetTimestamp(achievement.ID)));
    }

    std::sort(g_achievements.begin(), g_achievements.end(), [](const auto& a, const auto& b)
    {
        return std::get<1>(a) > std::get<1>(b);
    });

    ButtonGuide::Open("ButtonGuide_Back");

    ResetSelection();
    Game_PlaySound("sys_actstg_pausewinopen");

    hid::SetProhibitedInputs(XAMINPUT_GAMEPAD_START);
}

void AchievementMenu::Close()
{
    if (!g_isClosing)
    {
        g_appearTime = ImGui::GetTime();
        g_isClosing = true;

        hid::SetProhibitedInputs();
    }

    ButtonGuide::Close();

    Game_PlaySound("sys_actstg_pausewinclose");
    Game_PlaySound("sys_actstg_pausecansel");
}
