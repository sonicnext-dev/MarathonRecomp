#include "achievement_menu.h"
#include <gpu/imgui/imgui_snapshot.h>
#include <gpu/video.h>
#include <hid/hid.h>
#include <kernel/xdbf.h>
#include <locale/achievement_locale.h>
#include <locale/locale.h>
#include <patches/aspect_ratio_patches.h>
#include <patches/MainMenuTask_patches.h>
#include <ui/button_window.h>
#include <ui/imgui_utils.h>
#include <user/achievement_manager.h>
#include <user/config.h>
#include <app.h>
#include <exports.h>

static constexpr double CONTAINER_MOVE_OFFSET = 0;
static constexpr double CONTAINER_MOVE_DURATION = 5;
static constexpr double CONTAINER_FADE_OFFSET = CONTAINER_MOVE_OFFSET;
static constexpr double CONTAINER_FADE_DURATION = 10;
static constexpr double ROW_FADE_OFFSET = CONTAINER_MOVE_DURATION;
static constexpr double ROW_FADE_TIME = CONTAINER_MOVE_DURATION;

static constexpr int MAX_VISIBLE_ROWS = 4;

static double g_time{};
static double g_rowSelectionTime{};
static double g_scrollArrowsTime{};
static double g_lastIncrementTime{};
static double g_lastTappedTime{};

static bool g_up{};
static bool g_upWasHeld{};
static bool g_down{};
static bool g_downWasHeld{};
static bool g_hasSwitched{};

static int g_rowCount{};
static int g_selectedIndex{};

static std::vector<std::tuple<Achievement, time_t>> g_achievements{};

static void MoveCursor(int& cursorIndex, int min = 0, int max = INT_MAX)
{
    auto time = ImGui::GetTime();

    auto scrollUp = g_up;
    auto scrollDown = g_down;

    if (scrollUp || scrollDown)
        g_lastTappedTime = time;

    static constexpr auto FAST_SCROLL_THRESHOLD = 0.3;
    static constexpr auto FAST_SCROLL_SPEED = 1.0 / 6.5;

    auto fastScroll = (time - g_lastTappedTime) > FAST_SCROLL_THRESHOLD;

    if (fastScroll)
    {
        if ((time - g_lastIncrementTime) < FAST_SCROLL_SPEED)
        {
            fastScroll = false;
        }
        else
        {
            g_lastIncrementTime = time;

            scrollUp = g_upWasHeld;
            scrollDown = g_downWasHeld;
        }
    }

    if (scrollUp)
    {
        --cursorIndex;

        if (cursorIndex < min)
            cursorIndex = max - 1;
    }
    else if (scrollDown)
    {
        ++cursorIndex;

        if (cursorIndex >= max)
            cursorIndex = min;
    }

    if (scrollUp || scrollDown)
    {
        Game_PlaySound("move");

        g_rowSelectionTime = time;
        g_scrollArrowsTime = g_rowSelectionTime;
    }
}

static void DrawContainer(ImVec2 min, ImVec2 max)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    auto containerTopCornerUVs = PIXELS_TO_UV_COORDS(1024, 1024, 1, 400, 50, 50);
    auto containerTopCentreUVs = PIXELS_TO_UV_COORDS(1024, 1024, 50, 400, 50, 50);
    auto containerSideUVs = PIXELS_TO_UV_COORDS(1024, 1024, 1, 450, 50, 50);
    auto containerCentreUVs = PIXELS_TO_UV_COORDS(1024, 1024, 50, 450, 50, 50);
    auto containerAlphaMotionTime = AchievementMenu::IsClosing() ? 0 : ComputeLinearMotion(g_time, CONTAINER_FADE_OFFSET, CONTAINER_FADE_DURATION);
    auto containerColour = IM_COL32(255, 255, 255, AchievementMenu::s_state == AchievementMenuState::GoldMedals ? Lerp(63, 0, containerAlphaMotionTime) : 63);
    auto containerEdgeSize = Scale(50, true);

    ImVec2 containerTopLeftCornerMin = min;
    ImVec2 containerTopLeftCornerMax = { containerTopLeftCornerMin.x + containerEdgeSize, containerTopLeftCornerMin.y + containerEdgeSize };
    ImVec2 containerTopCentreCornerMin = { containerTopLeftCornerMax.x, containerTopLeftCornerMin.y };
    ImVec2 containerTopCentreCornerMax = { max.x - containerEdgeSize, containerTopCentreCornerMin.y + containerEdgeSize };
    ImVec2 containerTopRightCornerMin = { containerTopCentreCornerMax.x, containerTopCentreCornerMin.y };
    ImVec2 containerTopRightCornerMax = { containerTopRightCornerMin.x + containerEdgeSize, containerTopRightCornerMin.y + containerEdgeSize };
    ImVec2 containerLeftMin = { containerTopLeftCornerMin.x, containerTopLeftCornerMax.y };
    ImVec2 containerLeftMax = { containerTopLeftCornerMax.x, max.y };
    ImVec2 containerRightMin = { containerTopRightCornerMin.x, containerTopRightCornerMax.y };
    ImVec2 containerRightMax = { containerTopRightCornerMax.x, max.y };
    ImVec2 containerCentreMin = containerTopLeftCornerMax;
    ImVec2 containerCentreMax = { containerRightMin.x, containerRightMax.y };

    auto containerBottomFadeStart = Scale(40, true);
    auto containerBottomFadeEnd = Scale(10, true);
    ImVec2 containerBottomFadeMin = { containerLeftMin.x, containerLeftMax.y - containerBottomFadeStart };
    ImVec2 containerBottomFadeMax = { containerRightMax.x, containerRightMax.y + containerBottomFadeEnd };

    SetVerticalGradient(containerBottomFadeMin, containerBottomFadeMax, IM_COL32_WHITE, IM_COL32_WHITE_TRANS);
    drawList->AddImage(g_upTexMainMenu1.get(), containerTopLeftCornerMin, containerTopLeftCornerMax, GET_UV_COORDS(containerTopCornerUVs), containerColour);
    drawList->AddImage(g_upTexMainMenu1.get(), containerTopCentreCornerMin, containerTopCentreCornerMax, GET_UV_COORDS(containerTopCentreUVs), containerColour);
    AddImageFlipped(g_upTexMainMenu1.get(), containerTopRightCornerMin, containerTopRightCornerMax, GET_UV_COORDS(containerTopCornerUVs), containerColour, true);
    drawList->AddImage(g_upTexMainMenu1.get(), containerLeftMin, containerLeftMax, GET_UV_COORDS(containerSideUVs), containerColour);
    AddImageFlipped(g_upTexMainMenu1.get(), containerRightMin, containerRightMax, GET_UV_COORDS(containerSideUVs), containerColour, true);
    drawList->AddImage(g_upTexMainMenu1.get(), containerCentreMin, containerCentreMax, GET_UV_COORDS(containerCentreUVs), containerColour);
    ResetGradient();

    drawList->PushClipRect(containerTopLeftCornerMin, containerRightMax);
}

static void DrawAchievement(int rowIndex, Achievement& achievement, bool isUnlocked)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    auto clipRectMin = drawList->GetClipRectMin();
    auto clipRectMax = drawList->GetClipRectMax();

    auto itemHeight = Scale(94, true);
    auto itemMarginX = Scale(18, true);
    auto imageMarginX = Scale(34, true);
    auto imageMarginY = Scale(20, true);
    auto imageSize = Scale(64, true);

    auto offsetScroll = 0.0f;
    auto startIndex = 0;

    // Only scroll if page has more than four items.
    if (g_rowCount > MAX_VISIBLE_ROWS)
    {
        if (g_selectedIndex >= g_rowCount - (MAX_VISIBLE_ROWS - 1))
        {
            // Stop scrolling near bottom to use cursor instead.
            startIndex = g_rowCount - MAX_VISIBLE_ROWS;
            offsetScroll = -startIndex * itemHeight;
        }
        else if (g_selectedIndex >= 1)
        {
            // Start scrolling from the middle item.
            startIndex = g_selectedIndex - 1;
            offsetScroll = -startIndex * itemHeight;
        }
    }

    auto offsetY = itemHeight * rowIndex + offsetScroll;
    auto isCurrent = g_selectedIndex == rowIndex;
    auto isVisible = ((rowIndex - startIndex + g_rowCount) % g_rowCount) < MAX_VISIBLE_ROWS;

    if (!isVisible)
        return;

    ImVec2 min = { clipRectMin.x + itemMarginX, clipRectMin.y + offsetY };
    ImVec2 max = { clipRectMax.x + itemMarginX, min.y + itemHeight };

    auto rowUVs = PIXELS_TO_UV_COORDS(1024, 1024, 605, 449, 10, 30);
    auto rowMarginX = Scale(2, true);
    auto rowMarginY = Scale(24, true);

    auto rowColourBreatheMotionTime = BREATHE_MOTION(1.0f, 0.0f, g_rowSelectionTime, 0.9f);
    auto rowColourTransitionMotionTime = ComputeLinearMotion(g_time, ROW_FADE_OFFSET, ROW_FADE_TIME);

    constexpr auto rowColourAlphaMax = 165;
    constexpr auto rowColourAlphaMin = 115;

    auto rowColourAlpha = isCurrent
        ? Lerp(rowColourAlphaMax, rowColourAlphaMin, rowColourBreatheMotionTime) * rowColourTransitionMotionTime
        : rowColourAlphaMin * rowColourTransitionMotionTime;

    auto rowColour = IM_COL32(255, 255, 255, rowColourAlpha);

    ImVec2 rowMin = { clipRectMin.x + rowMarginX, min.y + itemHeight - rowMarginY };
    ImVec2 rowMax = { clipRectMax.x - rowMarginX, rowMin.y + Scale(30, true) };

    SetAdditive(true);
    SetVerticalGradient(rowMin, rowMax, IM_COL32_WHITE_TRANS, rowColour);
    drawList->AddImage(g_upTexMainMenu1.get(), rowMin, rowMax, GET_UV_COORDS(rowUVs), rowColour);
    ResetGradient();
    ResetAdditive();

    auto image = g_xdbfTextureCache[achievement.ID];
    ImVec2 imageMin = { min.x + imageMarginX, min.y + imageMarginY };
    ImVec2 imageMax = { min.x + imageMarginX + imageSize, min.y + imageMarginY + imageSize };

    if (isCurrent)
        DrawArrowCursor({ rowMin.x + Scale(2, true), imageMin.y + ((imageMax.y - imageMin.y) / 3)}, g_time, false);

    if (!isUnlocked)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_GRAYSCALE);

    // Draw achievement image.
    drawList->AddImage(image, imageMin, imageMax, { 0, 0 }, { 1, 1 }, IM_COL32_WHITE);

    if (!isUnlocked)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);

    auto fontSize = Scale(Config::Language == ELanguage::Japanese ? 28 : 27, true);

    auto textX = imageMax.x + itemMarginX * 2;
    auto textColour = isUnlocked ? IM_COL32_WHITE : IM_COL32(137, 137, 137, 255);

    std::string name;
    std::string lockedDesc;
    std::string unlockedDesc;

    if (Config::UseOfficialAchievementText)
    {
        name = achievement.Name;
        lockedDesc = achievement.LockedDesc;
        unlockedDesc = achievement.UnlockedDesc;
    }
    else
    {
        auto& newLocale = GetAchievementLocale(achievement.ID);

        name = newLocale.Name;
        lockedDesc = newLocale.LockedDesc;
        unlockedDesc = newLocale.UnlockedDesc;
    }

    auto descText = isUnlocked ? unlockedDesc.c_str() : lockedDesc.c_str();
    auto descTextOffsetY = Scale(32, true);
    auto descSize = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0, descText);

    ImVec2 namePos = { textX, imageMin.y + Scale(3, true) };
    ImVec2 descPos = { textX, namePos.y + descTextOffsetY };

    // Draw achievement name.
    SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);
    drawList->AddText(g_pFntRodin, fontSize, namePos, textColour, name.c_str());
    SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);

    auto timestampOffsetX = Scale(60, true);

    ImVec2 marqueeMin = { textX, min.y };
    ImVec2 marqueeMax = { max.x - timestampOffsetX, max.y };
    constexpr auto marqueeDelay = 0.9;
    auto shouldMarquee = isCurrent && marqueeMin.x + descSize.x >= marqueeMax.x;

    // Reduce clip rect size when marquee starts to make the
    // scrolling text align more with text above or below it.
    if (shouldMarquee && ImGui::GetTime() - (g_rowSelectionTime + marqueeDelay) > 0.0)
    {
        marqueeMin.x += Scale(2, true);
        marqueeMax.x -= Scale(1, true);
    }

    SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);
    DrawTextWithMarquee(g_pFntRodin, fontSize, descPos, marqueeMin, marqueeMax, textColour, descText, g_rowSelectionTime, marqueeDelay, shouldMarquee ? Scale(200, true) : 0);
    SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);

    if (!isUnlocked)
        return;

    auto timestamp = AchievementManager::GetTimestamp(achievement.ID);

    if (!timestamp)
        return;

    char buffer[32];

#ifdef _WIN32
    tm time{};
    tm* pTime = &time;
    localtime_s(pTime, &timestamp);
#else
    tm* pTime = localtime(&timestamp);
#endif

    snprintf(buffer, sizeof(buffer), "%d/%d/%d %02d:%02d",
        pTime->tm_year + 1900, pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min);
    
    auto timestampSize = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0, buffer);

    // Draw timestamp text.
    SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);
    drawList->AddText(g_pFntRodin, fontSize, { max.x - timestampSize.x - timestampOffsetX, namePos.y }, IM_COL32_WHITE, buffer);
    SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);
}

void AchievementMenu::Draw()
{
    if (AchievementMenu::IsClosing() && s_commonMenu.Close() >= 1.0)
    {
        s_isVisible = false;
        return;
    }

    if (!s_isVisible)
        return;

    auto* drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;

    if (s_commonMenu.ShowDescription)
    {
        auto horzMargin = Scale(128, true);
        auto gradientTop = IM_COL32(0, 103, 255, 255);
        auto gradientBottom = IM_COL32(0, 41, 100, 255);

        ImVec2 footerClipMin = { g_horzCentre + horzMargin, res.y - g_vertCentre - Scale(152, true) };
        ImVec2 footerClipMax = { res.x - g_horzCentre - horzMargin, res.y - g_vertCentre - Scale(107, true) };

        drawList->PushClipRect(footerClipMin, footerClipMax);
        drawList->AddRectFilledMultiColor({ 0.0f, g_vertCentre }, { res.x, res.y - g_vertCentre }, gradientTop, gradientTop, gradientBottom, gradientBottom);
        drawList->PopClipRect();
    }

    s_commonMenu.Draw();

    if (s_pMainMenuTask && s_pMainMenuTask->m_State == Sonicteam::MainMenuTask::MainMenuState_GoldMedalResults)
    {
        if (auto& spInputManager = App::s_pApp->m_pDoc->m_vspInputManager[0])
        {
            auto& rPadState = spInputManager->m_PadState;

            if (rPadState.IsPressed(Sonicteam::SoX::Input::KeyState_B))
            {
                AchievementMenu::Close();
                ButtonWindow::Close();
            }

            switch (AchievementMenu::s_state)
            {
                case AchievementMenuState::GoldMedals:
                {
                    if (rPadState.IsPressed(Sonicteam::SoX::Input::KeyState_Y))
                    {
                        AchievementMenu::SetState(AchievementMenuState::Achievements);
                        ButtonWindow::Open("Button_GoldMedalsBack");
                        SetGoldMedalResultsVisible(false);
                        Game_PlaySound("window_open");
                        g_hasSwitched = true;
                    }

                    break;
                }

                case AchievementMenuState::Achievements:
                {
                    // Discard all buttons besides B.
                    if ((s_pMainMenuTask->m_PressedButtons.get() & 0x20) == 0)
                        s_pMainMenuTask->m_PressedButtons = 0;

                    if (rPadState.IsPressed(Sonicteam::SoX::Input::KeyState_Y))
                    {
                        AchievementMenu::SetState(AchievementMenuState::GoldMedals);
                        ButtonWindow::Open("Button_AchievementsBack");
                        SetGoldMedalResultsVisible(true);
                        Game_PlaySound("window_close");
                    }

                    break;
                }
            }
        }
    }

    auto containerMotionTime = ComputeLinearMotion(g_time, CONTAINER_MOVE_OFFSET, CONTAINER_MOVE_DURATION, s_state == AchievementMenuState::GoldMedals);
    auto containerTop = Scale(Lerp(217, 148, containerMotionTime), true);
    auto containerBottom = Scale(554, true);
    auto containerWidth = Scale(1158, true);

    ImVec2 min = { (res.x / 2) - containerWidth / 2, g_vertCentre + containerTop };
    ImVec2 max = { min.x + containerWidth, g_vertCentre + containerBottom };

    switch (s_state)
    {
        case AchievementMenuState::GoldMedals:
        {
            s_commonMenu.SetTitle(Localise("Achievements_GoldMedals_Uppercase"));
            s_commonMenu.ShowDescription = false;
            
            if (g_hasSwitched)
            {
                // For outro animation.
                DrawContainer(min, max);
                drawList->PopClipRect();
            }

            break;
        }

        case AchievementMenuState::Achievements:
        {
            s_commonMenu.SetTitle(Localise("Achievements_Title_Uppercase"));
            s_commonMenu.ShowDescription = true;

            DrawContainer(min, max);

            if (containerMotionTime >= 1.0)
            {
                auto rowIndex = 0;

                for (auto& tpl : g_achievements)
                {
                    auto& achievement = std::get<0>(tpl);

                    if (AchievementManager::IsUnlocked(achievement.ID))
                        DrawAchievement(rowIndex++, achievement, true);
                }

                for (auto& tpl : g_achievements)
                {
                    auto& achievement = std::get<0>(tpl);

                    if (!AchievementManager::IsUnlocked(achievement.ID))
                        DrawAchievement(rowIndex++, achievement, false);
                }

                drawList->PopClipRect();

                auto scrollArrowsOffsetX = Scale(17, true);
                auto scrollArrowsOffsetTop = Scale(40, true);
                auto scrollArrowsOffsetBottom = Scale(60, true);
                ImVec2 scrollArrowsMin = { max.x + scrollArrowsOffsetX, min.y + scrollArrowsOffsetTop };
                ImVec2 scrollArrowsMax = { scrollArrowsMin.x + scrollArrowsOffsetX, max.y - scrollArrowsOffsetBottom };

                DrawScrollArrows(scrollArrowsMin, scrollArrowsMax, Scale(25, true), g_scrollArrowsTime, g_selectedIndex > MAX_VISIBLE_ROWS / 2, g_selectedIndex < g_rowCount - MAX_VISIBLE_ROWS / 2);

                auto upIsHeld = false;
                auto downIsHeld = false;

                for (auto& spInputManager : App::s_pApp->m_pDoc->m_vspInputManager)
                {
                    auto& rPadState = spInputManager->m_PadState;

                    if (rPadState.IsDown(Sonicteam::SoX::Input::KeyState_DPadUp) || -rPadState.LeftStickVertical > 0.5f)
                        upIsHeld = true;

                    if (!g_upWasHeld && upIsHeld)
                        g_up = true;

                    if (rPadState.IsDown(Sonicteam::SoX::Input::KeyState_DPadDown) || -rPadState.LeftStickVertical < -0.5f)
                        downIsHeld = true;

                    if (!g_downWasHeld && downIsHeld)
                        g_down = true;
                }

                MoveCursor(g_selectedIndex, 0, g_rowCount);

                g_up = false;
                g_upWasHeld = upIsHeld;
                g_down = false;
                g_downWasHeld = downIsHeld;
            }

            drawList->PopClipRect();

            break;
        }
    }
}

void AchievementMenu::Open(Sonicteam::MainMenuTask* pMainMenuTask)
{
    s_pMainMenuTask = pMainMenuTask;

    if (s_isVisible)
        return;

    g_achievements.clear();

    for (auto& achievement : g_xdbfWrapper.GetAchievements((EXDBFLanguage)Config::Language.Value))
    {
        achievement.LockedDesc = xdbf::FixInvalidSequences(achievement.LockedDesc);

        g_achievements.push_back(std::make_tuple(achievement, AchievementManager::GetTimestamp(achievement.ID)));
    }

    std::sort(g_achievements.begin(), g_achievements.end(), [](const auto& a, const auto& b)
    {
        return std::get<1>(a) > std::get<1>(b);
    });

    g_rowCount = g_achievements.size();

    // Format achievement progress.
    char descriptionText[128];
    snprintf(descriptionText, sizeof(descriptionText), Localise("Achievements_Progress").c_str(), AchievementManager::GetTotalRecords(), g_rowCount);

    s_commonMenu = CommonMenu(Localise("Achievements_GoldMedals_Uppercase"), descriptionText, false);
    s_commonMenu.ShowDescription = false;
    s_commonMenu.ShowVersionString = false;
    s_commonMenu.ReduceDraw = true;
    s_commonMenu.Open();

    s_isVisible = true;
    s_state = AchievementMenuState::GoldMedals;
    g_time = ImGui::GetTime();
    g_hasSwitched = false;
    g_selectedIndex = 0;

    ButtonWindow::Open("Button_AchievementsBack");
    MainMenuTaskPatches::s_hideButtonWindow = true;
}

void AchievementMenu::Close()
{
    if (AchievementMenu::IsClosing())
        return;

    switch (s_state)
    {
        case AchievementMenuState::GoldMedals:
            s_state = AchievementMenuState::ClosingGoldMedals;
            break;

        case AchievementMenuState::Achievements:
            s_state = AchievementMenuState::ClosingAchievements;
            break;
    }

    g_time = ImGui::GetTime();

    ButtonWindow::Close();
    MainMenuTaskPatches::s_hideButtonWindow = false;
}

void AchievementMenu::SetState(AchievementMenuState state)
{
    s_state = state;
    g_time = ImGui::GetTime();
    g_rowSelectionTime = g_time;
}

bool AchievementMenu::IsClosing()
{
    return s_state == AchievementMenuState::ClosingGoldMedals || s_state == AchievementMenuState::ClosingAchievements;
}

void AchievementMenu::SetGoldMedalResultsVisible(bool isVisible)
{
    if (!s_pMainMenuTask)
        return;

    std::vector<int> states;

    if (isVisible)
    {
        states = { 0, 3, 5, 11 };
    }
    else
    {
        states = { 1, 4, 6, 12 };
    }

    for (auto& state : states)
    {
        guest_stack_var<Sonicteam::Message::HUDGoldMedal::MsgChangeState> msgChangeState(state, s_pMainMenuTask->m_GoldMedalEpisodeIndex);
        s_pMainMenuTask->m_pHUDGoldMedal->ProcessMessage(msgChangeState.get());
    }

    for (int i = 0; i < 5; i++)
    {
        auto& spTextEntity = s_pMainMenuTask->m_pHUDGoldMedal->m_aspTextEntities[i];

        if (auto pTextEntity = spTextEntity.get())
        {
            for (size_t i = 0; i < pTextEntity->m_CharacterVertexCount; i++)
                pTextEntity->m_pCharacterVertices[i].Colour = isVisible ? 0xFFFFFFFF : 0x00FFFFFF;
        }
    }
}
