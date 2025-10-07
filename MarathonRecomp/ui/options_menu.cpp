#include "options_menu.h"
#include <locale/locale.h>
#include <patches/aspect_ratio_patches.h>
#include <patches/audio_patches.h>
#include <res/images/common/main_menu7.dds.h>
#include <res/images/common/main_menu8.dds.h>
#include <res/images/common/main_menu9.dds.h>
#include <ui/game_window.h>
#include <ui/imgui_utils.h>
#include <user/config.h>
#include <app.h>
#include <decompressor.h>
#include <exports.h>

// #define USE_REFERENCE_PARAMETERS

#ifdef USE_REFERENCE_PARAMETERS
static constexpr double CTRL_OFFSET_X = 5.0;
#else
static constexpr double CTRL_OFFSET_X = 30.0;
#endif

static double g_stateTime{};
static double g_flowStateTime{};
static double g_cursorArrowsTime{};
static double g_scrollArrowsTime{};

static bool g_up{};
static bool g_upWasHeld{};
static bool g_down{};
static bool g_downWasHeld{};
static bool g_left{};
static bool g_leftWasHeld{};
static bool g_right{};
static bool g_rightWasHeld{};
static bool g_isAccepted{};
static bool g_isDeclined{};
static bool g_isReset{};

static int g_categoryIndex{ -1 };
static int g_optionIndex{};
static int g_optionCount{};
static IConfigDef* g_optionCurrent{};
static bool g_optionCanReset{};

static std::unique_ptr<GuestTexture> g_upTexMainMenu7{};
static std::unique_ptr<GuestTexture> g_upTexMainMenu8{};
static std::unique_ptr<GuestTexture> g_upTexMainMenu9{};

static std::string& GetCategoryName(OptionsMenuCategory category)
{
    switch (category)
    {
        case OptionsMenuCategory::System: return Localise("Options_Category_System");
        case OptionsMenuCategory::Input: return Localise("Options_Category_Input");
        case OptionsMenuCategory::Audio: return Localise("Options_Category_Audio");
        case OptionsMenuCategory::Video: return Localise("Options_Category_Video");
        case OptionsMenuCategory::Code: return Localise("Options_Category_Code");
    }

    return g_localeMissing;
}

static std::string& GetCategoryDescription(OptionsMenuCategory category)
{
    switch (category)
    {
        case OptionsMenuCategory::System: return Localise("Options_Desc_Category_System");
        case OptionsMenuCategory::Input: return Localise("Options_Desc_Category_Input");
        case OptionsMenuCategory::Audio: return Localise("Options_Desc_Category_Audio");
        case OptionsMenuCategory::Video: return Localise("Options_Desc_Category_Video");
        case OptionsMenuCategory::Code: return Localise("Options_Desc_Category_Code");
    }

    return g_localeMissing;
}

static void ResetCategorySelection()
{
    g_categoryIndex = -1;
}

static void ResetOptionSelection()
{
    g_optionIndex = 0;
    g_optionCurrent = nullptr;
}

static void ResetSelection()
{
    g_flowStateTime = 0.0;

    ResetCategorySelection();
    ResetOptionSelection();
}

static bool CheckAndDiscard(bool& value)
{
    if (value)
    {
        value = false;
        return true;
    }

    return false;
}

static void MoveCursor(int& cursorIndex, double& cursorTime, int min = 0, int max = INT_MAX, std::function<void()> onCursorMoved = nullptr)
{
    auto time = ImGui::GetTime();

    // TODO: handle holding inputs and allow selecting up/down or left/right.

    if (g_up)
    {
        --cursorIndex;

        if (cursorIndex < min)
            cursorIndex = max - 1;
    }
    else if (g_down)
    {
        ++cursorIndex;

        if (cursorIndex >= max)
            cursorIndex = min;
    }

    if (g_up || g_down)
    {
        cursorTime = ImGui::GetTime();

        Game_PlaySound("move");

        if (onCursorMoved)
            onCursorMoved();
    }
}

static void DrawCategories(ImVec2 min, ImVec2 max)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    auto selectedUVs = PIXELS_TO_UV_COORDS(1024, 1024, 443, 524, 560, 47);
    auto unselectedUVs = PIXELS_TO_UV_COORDS(1024, 1024, 443, 579, 560, 47);
    auto categoryWidth = Scale(560, true);
    auto categoryHeight = Scale(47, true);

    if (g_categoryIndex == -1)
    {
        g_categoryIndex = 0;
        OptionsMenu::s_commonMenu.SetDescription(GetCategoryDescription((OptionsMenuCategory)g_categoryIndex));
    }

    for (size_t i = 0; i < (size_t)OptionsMenuCategory::Count; i++)
    {
        // Don't show codes category if locked.
        if (!OptionsMenu::s_isCodesUnlocked && (OptionsMenuCategory)i == OptionsMenuCategory::Code)
            continue;

        auto isCurrent = i == g_categoryIndex;
        auto categoryOffsetY = Scale(48, true) * i;

        ImVec2 categoryMin = { min.x + Scale(42, true), min.y + Scale(147, true) + categoryOffsetY };
        ImVec2 categoryMax = { categoryMin.x + categoryWidth, categoryMin.y + categoryHeight };

        auto categoryAlphaMotionTime = ComputeMotion(g_stateTime, 0, 5);
        auto categoryAlphaMotion = IM_COL32(255, 255, 255, 255 * categoryAlphaMotionTime);

        SetHorizontalGradient(categoryMin, categoryMax, categoryAlphaMotion, IM_COL32_WHITE_TRANS);
        drawList->AddImage(g_upTexMainMenu7.get(), categoryMin, categoryMax, GET_UV_COORDS(isCurrent ? selectedUVs : unselectedUVs));
        ResetGradient();

        if (isCurrent && OptionsMenu::s_flowState == OptionsMenuFlowState::CategoryCursor)
        {
            auto cursorOffsetX = Scale(80, true);
            auto cursorOffsetY = Scale(8, true);

            DrawArrowCursor({ categoryMin.x + cursorOffsetX, categoryMin.y + cursorOffsetY }, g_cursorArrowsTime, true, false, OptionsMenu::s_state == OptionsMenuState::Closing);
        }

        auto fontSize = Scale(27, true);
        auto text = GetCategoryName((OptionsMenuCategory)i);
        auto textSize = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text.c_str());

        SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);
        drawList->AddText(g_pFntRodin, fontSize, { categoryMin.x + Scale(129, true), categoryMin.y + Scale(6, true) }, categoryAlphaMotion, text.c_str());
        SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);
    }
}

template <typename T, bool isHidden = false>
static void DrawOption(int rowIndex, ConfigDef<T, isHidden>* config, bool isAccessible, std::string* inaccessibleReason = nullptr, T valueMin = T(0), T valueCentre = T(0.5), T valueMax = T(1), bool isSlider = true)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    auto clipRectMin = drawList->GetClipRectMin();
    auto clipRectMax = drawList->GetClipRectMax();

    auto fontSize = Scale(27, true);
    auto accessibleColour = isAccessible ? IM_COL32_WHITE : IM_COL32(137, 137, 137, 255);
    auto optionHeight = Scale(106, true);
    auto offsetScroll = 0.0f;

    // Only scroll if page has more than three items.
    if (g_optionCount > 3)
    {
        if (g_optionIndex >= g_optionCount - 2)
        {
            // Stop scrolling near bottom to use cursor instead.
            offsetScroll = -(g_optionCount - 3) * optionHeight;
        }
        else if (g_optionIndex >= 1)
        {
            // Start scrolling from the middle item.
            offsetScroll = -(g_optionIndex - 1) * optionHeight;
        }
    }

    auto offsetY = optionHeight * rowIndex + offsetScroll;
    auto isCurrent = g_optionIndex == rowIndex;
    auto isSelected = isCurrent && OptionsMenu::s_flowState == OptionsMenuFlowState::OptionSelected;

    auto bgEdgeUVs = PIXELS_TO_UV_COORDS(256, 256, 1, 0, 51, 45);
    auto bgStretchUVs = PIXELS_TO_UV_COORDS(256, 256, 51, 0, 51, 45);
    auto bgWidth = Scale(50, true);
    auto bgHeight = Scale(45, true);
    auto bgColourMotion = isSelected ? 255 : 0;
    auto bgColour = IM_COL32(bgColourMotion, bgColourMotion, bgColourMotion, 245);
    auto bgFadeColour = IM_COL32(bgColourMotion, bgColourMotion, bgColourMotion, 45);

    ImVec2 titleBgEdgeMin = { clipRectMin.x, clipRectMin.y + offsetY };
    ImVec2 titleBgEdgeMax = { titleBgEdgeMin.x + bgWidth, titleBgEdgeMin.y + bgHeight };
    ImVec2 titleBgStretchMin = { titleBgEdgeMax.x, titleBgEdgeMin.y };
    ImVec2 titleBgStretchMax = { clipRectMax.x, titleBgEdgeMax.y };
    ImVec2 titleBgFadeMin = { titleBgStretchMax.x - Scale(300, true), titleBgStretchMax.y };
    ImVec2 titleBgFadeMax = titleBgStretchMax;

    drawList->AddImage(g_upTexMainMenu8.get(), titleBgEdgeMin, titleBgEdgeMax, GET_UV_COORDS(bgEdgeUVs), bgColour);

    SetHorizontalGradient(titleBgFadeMin, titleBgFadeMax, bgColour, bgFadeColour);
    drawList->AddImage(g_upTexMainMenu8.get(), titleBgStretchMin, titleBgStretchMax, GET_UV_COORDS(bgStretchUVs), bgColour);
    ResetGradient();

    ImVec2 titlePos = { titleBgEdgeMin.x + Scale(51, true), titleBgEdgeMin.y + Scale(8, true) };

    SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);
    drawList->AddText(g_pFntRodin, fontSize, titlePos, accessibleColour, config->GetNameLocalised(Config::Language).c_str());
    SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);

    if (isCurrent)
    {
        if (OptionsMenu::s_flowState == OptionsMenuFlowState::OptionCursor)
        {
            if (config != g_optionCurrent)
                OptionsMenu::s_commonMenu.SetDescription(isAccessible ? config->GetDescription(Config::Language) : *inaccessibleReason);

            DrawArrowCursor({ titleBgEdgeMin.x + Scale(8, true), titleBgEdgeMin.y + Scale(9, true)}, g_cursorArrowsTime, true, false, OptionsMenu::s_state == OptionsMenuState::Closing);

            g_optionCurrent = config;
        }
        else
        {
            g_optionCurrent = nullptr;
        }
    }

    ImVec2 ctrlBgLeftEdgeMin = { titleBgEdgeMin.x + Scale(CTRL_OFFSET_X, true), titleBgEdgeMin.y + Scale(54, true)};
    ImVec2 ctrlBgLeftEdgeMax = { ctrlBgLeftEdgeMin.x + bgWidth, ctrlBgLeftEdgeMin.y + bgHeight };
    ImVec2 ctrlBgStretchMin = { ctrlBgLeftEdgeMax.x, ctrlBgLeftEdgeMin.y };
    ImVec2 ctrlBgStretchMax = { ctrlBgStretchMin.x + Scale(300, true), ctrlBgLeftEdgeMax.y };
    ImVec2 ctrlBgRightEdgeMin = { ctrlBgStretchMax.x, ctrlBgStretchMin.y };
    ImVec2 ctrlBgRightEdgeMax = { ctrlBgRightEdgeMin.x + bgWidth, ctrlBgRightEdgeMin.y + bgHeight };

    ImVec2 ctrlBgCentre =
    {
        ctrlBgLeftEdgeMin.x + ((ctrlBgRightEdgeMax.x - ctrlBgLeftEdgeMin.x) / 2),
        ctrlBgLeftEdgeMin.y + ((ctrlBgLeftEdgeMax.y - ctrlBgLeftEdgeMin.y) / 2)
    };

    drawList->AddImage(g_upTexMainMenu8.get(), ctrlBgLeftEdgeMin, ctrlBgLeftEdgeMax, GET_UV_COORDS(bgEdgeUVs), accessibleColour);
    drawList->AddImage(g_upTexMainMenu8.get(), ctrlBgStretchMin, ctrlBgStretchMax, GET_UV_COORDS(bgStretchUVs), accessibleColour);
    AddImageFlipped(g_upTexMainMenu8.get(), ctrlBgRightEdgeMin, ctrlBgRightEdgeMax, GET_UV_COORDS(bgEdgeUVs), accessibleColour, true);

    constexpr auto isSliderType = std::is_same_v<T, float> || std::is_same_v<T, int>;

    auto drawSelectionArrows = [=]()
    {
        if (!isSelected)
            return;

        auto arrowUVs = PIXELS_TO_UV_COORDS(256, 256, 15, 46, 28, 26);
        auto arrowWidth = Scale(28, true);
        auto arrowHeight = Scale(26, true);
        auto arrowOffsetX = Scale(30, true);
        auto arrowOffsetY = Scale(10, true);

        ImVec2 arrowLeftMin = { ctrlBgLeftEdgeMin.x + arrowOffsetX, ctrlBgLeftEdgeMin.y + arrowOffsetY };
        ImVec2 arrowLeftMax = { arrowLeftMin.x + arrowWidth, arrowLeftMin.y + arrowHeight };
        ImVec2 arrowRightMin = { ctrlBgRightEdgeMax.x - arrowWidth - arrowOffsetX, arrowLeftMin.y };
        ImVec2 arrowRightMax = { arrowRightMin.x + arrowWidth, arrowLeftMax.y };

        // Draw left arrow.
        AddImageFlipped(g_upTexMainMenu8.get(), arrowLeftMin, arrowLeftMax, GET_UV_COORDS(arrowUVs), IM_COL32_WHITE, true);

        // Draw right arrow.
        drawList->AddImage(g_upTexMainMenu8.get(), arrowRightMin, arrowRightMax, GET_UV_COORDS(arrowUVs));
    };

    if constexpr (isSliderType)
    {
        if (isSlider)
        {
            auto gaugeOffsetX = Scale(41, true);
            auto gaugeHeight = Scale(11, true);

            ImVec2 gaugeMin = { ctrlBgLeftEdgeMin.x + gaugeOffsetX, (ctrlBgRightEdgeMin.y + bgHeight / 2) - (gaugeHeight / 2) + Scale(1, true) };
            ImVec2 gaugeMax = { ctrlBgRightEdgeMax.x - gaugeOffsetX, gaugeMin.y + gaugeHeight / 2 };

            drawList->AddRectFilled(gaugeMin, gaugeMax, IM_COL32_BLACK, Scale(10, true));

            auto handleUVs = PIXELS_TO_UV_COORDS(256, 256, 72, 49, 49, 19);
            auto handleWidth = Scale(49, true);
            auto handleHeight = Scale(19, true);
            auto handleOffsetX = Scale(12, true);
            auto handleOffsetFactor = 0.0f;

            if (config->Value <= valueCentre)
            {
                handleOffsetFactor = float(config->Value - valueMin) / (valueCentre - valueMin) * 0.5f;
            }
            else
            {
                handleOffsetFactor = 0.5f + float(config->Value - valueCentre) / (valueMax - valueCentre) * 0.5f;
            }

            handleOffsetX = ((gaugeMin.x + handleOffsetX) + ((gaugeMax.x - handleOffsetX) - (gaugeMin.x + handleOffsetX)) * handleOffsetFactor) - (handleWidth / 2);

            ImVec2 handleMin = { handleOffsetX, gaugeMin.y - (handleHeight / 2) + Scale(4.5, true) };
            ImVec2 handleMax = { handleMin.x + handleWidth, handleMin.y + handleHeight };

            drawList->AddImage(g_upTexMainMenu8.get(), handleMin, handleMax, GET_UV_COORDS(handleUVs), accessibleColour);
        }
        else
        {
            drawSelectionArrows();
        }
    }
    else
    {
        drawSelectionArrows();
    }

    if (isCurrent)
    {
        static T s_oldValue;

        if (isAccessible)
        {
            if (CheckAndDiscard(g_isAccepted))
            {
                Game_PlaySound("main_deside");

                if (OptionsMenu::s_flowState == OptionsMenuFlowState::OptionCursor)
                {
                    s_oldValue = config->Value;

                    if (config->LockCallback)
                        config->LockCallback(config);

                    OptionsMenu::s_flowState = OptionsMenuFlowState::OptionSelected;
                }
                else
                {
                    if (config->Value != s_oldValue)
                    {
                        VideoConfigValueChangedCallback(config);

                        if (config->ApplyCallback)
                            config->ApplyCallback(config);
                    }

                    OptionsMenu::s_flowState = OptionsMenuFlowState::OptionCursor;
                }

                g_flowStateTime = ImGui::GetTime();
            }

            if (CheckAndDiscard(g_isDeclined))
            {
                Game_PlaySound("window_close");

                if (config->Value != s_oldValue)
                {
                    config->Value = s_oldValue;

                    VideoConfigValueChangedCallback(config);

                    if (config->Callback)
                        config->Callback(config);

                    if (config->ApplyCallback)
                        config->ApplyCallback(config);
                }

                OptionsMenu::s_flowState = OptionsMenuFlowState::OptionCursor;
                g_flowStateTime = ImGui::GetTime();
            }

            if (g_optionCanReset && CheckAndDiscard(g_isReset))
            {
                Game_PlaySound("window_close");

                if (!config->IsDefaultValue())
                {
                    config->MakeDefault();

                    VideoConfigValueChangedCallback(config);

                    if (config->Callback)
                        config->Callback(config);

                    if (config->ApplyCallback)
                        config->ApplyCallback(config);
                }
            }

            if (isSelected)
            {
                auto increment = g_right;
                auto decrement = g_left;

                if constexpr (std::is_enum_v<T>)
                {
                    auto it = config->EnumTemplateReverse.find(config->Value);

                    if (decrement)
                    {
                        if (it == config->EnumTemplateReverse.begin())
                            it = config->EnumTemplateReverse.end();

                        --it;
                    }
                    else if (increment)
                    {
                        ++it;

                        if (it == config->EnumTemplateReverse.end())
                            it = config->EnumTemplateReverse.begin();
                    }

                    config->Value = it->first;
                    config->SnapToNearestAccessibleValue(increment);

                    if (increment || decrement)
                    {
                        Game_PlaySound("move");

                        if (config->Callback)
                            config->Callback(config);
                    }
                }
                else if constexpr (isSliderType)
                {
                    if constexpr (std::is_integral_v<T>)
                    {
                        if (decrement)
                            config->Value -= 1;
                        else if (increment)
                            config->Value += 1;
                    }
                    else
                    {
                        if (decrement)
                            config->Value -= 0.01f;
                        else if (increment)
                            config->Value += 0.01f;
                    }

                    auto isValueInBounds = config->Value >= valueMin && config->Value <= valueMax;

                    if ((increment || decrement) && isValueInBounds)
                        Game_PlaySound("move");

                    config->Value = std::clamp(config->Value, valueMin, valueMax);
                }
                else if constexpr (std::is_same_v<T, bool>)
                {
                    if (increment || decrement)
                    {
                        Game_PlaySound("move");

                        config->Value = !config->Value;
                    }
                }

                // Run standard callback if there's no callback on apply.
                if (!config->ApplyCallback)
                {
                    if ((increment || decrement) && config->Callback)
                        config->Callback(config);
                }
            }
        }
        else
        {
            if (CheckAndDiscard(g_isAccepted))
                Game_PlaySound("cannot_deside");
        }
    }

    std::string valueText;
    auto isValueCentred = false;

    if constexpr (std::is_same_v<T, float>)
    {
        valueText = fmt::format("{}%", int(round(config->Value * 100.0f)));
    }
    else if constexpr (std::is_same_v<T, int32_t>)
    {
        if (config == &Config::WindowSize)
        {
            if (Config::Fullscreen)
            {
                int displayW, displayH;

                GameWindow::GetSizeInPixels(&displayW, &displayH);

                valueText = fmt::format("{}x{}", displayW, displayH);
            }
            else
            {
                auto displayModes = GameWindow::GetDisplayModes();

                if (config->Value >= 0 && config->Value < displayModes.size())
                {
                    auto& displayMode = displayModes[config->Value];

                    valueText = fmt::format("{}x{}", displayMode.w, displayMode.h);
                }
                else
                {
                    valueText = fmt::format("{}x{}", GameWindow::s_width, GameWindow::s_height);
                }
            }

            isValueCentred = true;
        }
        else if (config == &Config::Monitor)
        {
            valueText = fmt::format("{}", config->Value + 1);
            isValueCentred = true;
        }
        else
        {
            valueText = fmt::format("{}", config->Value);

            if (isSlider && config->Value >= valueMax)
                valueText = Localise("Options_Value_Max");
        }
    }
    else
    {
        valueText = config->GetValueLocalised(Config::Language);
        isValueCentred = true;
    }

    auto valueTextSize = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, valueText.data());

    ImVec2 valuePos = { ctrlBgCentre.x - (valueTextSize.x / 2), ctrlBgCentre.y - (valueTextSize.y / 2) - Scale(2, true) };

    // Align text to right side of the background.
    if (!isValueCentred)
        valuePos.x = ctrlBgRightEdgeMax.x + Scale(10, true);

    SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);
    drawList->AddText(g_pFntRodin, fontSize, valuePos, accessibleColour, valueText.data());
    SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);
}

static void DrawOptions(ImVec2 min, ImVec2 max)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    drawList->PushClipRect(min, max);

    auto rowCount = 0;
    auto cmnReason = &Localise("Options_Desc_NotAvailable");

    switch ((OptionsMenuCategory)g_categoryIndex)
    {
        case OptionsMenuCategory::System:
            DrawOption(rowCount++, &Config::Language, !OptionsMenu::s_isPause, cmnReason);
            DrawOption(rowCount++, &Config::VoiceLanguage, !OptionsMenu::s_isPause, cmnReason);
            DrawOption(rowCount++, &Config::Subtitles, true);
            DrawOption(rowCount++, &Config::Hints, true);
            DrawOption(rowCount++, &Config::ControlTutorial, true);
            DrawOption(rowCount++, &Config::Autosave, true);
            DrawOption(rowCount++, &Config::AchievementNotifications, true);
            break;

        case OptionsMenuCategory::Input:
            DrawOption(rowCount++, &Config::HorizontalCamera, true);
            DrawOption(rowCount++, &Config::VerticalCamera, true);
            DrawOption(rowCount++, &Config::AllowBackgroundInput, true);
            DrawOption(rowCount++, &Config::ControllerIcons, true);
            break;

        case OptionsMenuCategory::Audio:
            DrawOption(rowCount++, &Config::MasterVolume, true);
            DrawOption(rowCount++, &Config::MusicVolume, true);
            DrawOption(rowCount++, &Config::EffectsVolume, true);
            DrawOption(rowCount++, &Config::ChannelConfiguration, !OptionsMenu::s_isPause, cmnReason);
            DrawOption(rowCount++, &Config::MusicAttenuation, AudioPatches::CanAttenuate(), &Localise("Options_Desc_OSNotSupported"));
            break;

        case OptionsMenuCategory::Video:
        {
            DrawOption(rowCount++, &Config::WindowSize, !Config::Fullscreen,
                &Localise("Options_Desc_NotAvailableFullscreen"),
                0, 0, (int)GameWindow::GetDisplayModes().size() - 1, false);

            auto displayCount = GameWindow::GetDisplayCount();
            auto canChangeMonitor = Config::Fullscreen && displayCount > 1;
            auto monitorReason = &Localise("Options_Desc_NotAvailableWindowed");

            if (Config::Fullscreen && displayCount <= 1)
                monitorReason = &Localise("Options_Desc_NotAvailableHardware");

            DrawOption(rowCount++, &Config::Monitor, canChangeMonitor, monitorReason, 0, 0, displayCount - 1, false);
            DrawOption(rowCount++, &Config::AspectRatio, true);
            DrawOption(rowCount++, &Config::ResolutionScale, true, nullptr, 0.25f, 1.0f, 2.0f);
            DrawOption(rowCount++, &Config::Fullscreen, true);
            DrawOption(rowCount++, &Config::VSync, true);
            DrawOption(rowCount++, &Config::FPS, true, nullptr, FPS_MIN, 120, FPS_MAX);
            DrawOption(rowCount++, &Config::Brightness, true);
            DrawOption(rowCount++, &Config::AntiAliasing, Config::AntiAliasing.InaccessibleValues.size() != 3, &Localise("Options_Desc_NotAvailableHardware"));
            DrawOption(rowCount++, &Config::TransparencyAntiAliasing, Config::AntiAliasing != EAntiAliasing::Off, &Localise("Options_Desc_NotAvailableMSAA"));
            DrawOption(rowCount++, &Config::ShadowResolution, true);
            DrawOption(rowCount++, &Config::ReflectionResolution, true);
            DrawOption(rowCount++, &Config::RadialBlur, true);
            DrawOption(rowCount++, &Config::CutsceneAspectRatio, true);
            DrawOption(rowCount++, &Config::UIAlignmentMode, true);

            break;
        }

        case OptionsMenuCategory::Code:
        {
            for (auto def : g_configDefinitions)
            {
                if (def->GetSection() != "Codes")
                    continue;

                DrawOption(rowCount++, (ConfigDef<bool, true>*)def, true);
            }

            break;
        }
    }

    g_optionCount = rowCount;

    drawList->PopClipRect();

    if (OptionsMenu::s_flowState == OptionsMenuFlowState::OptionCursor)
    {
        auto scrollArrowUVs = PIXELS_TO_UV_COORDS(1024, 1024, 500, 450, 50, 50);
        auto scrollArrowScale = Scale(20, true);
        auto scrollArrowAlphaMotionInTime = ComputeMotion(g_scrollArrowsTime, 0, 3);
        auto scrollArrowAlphaMotionPauseTime = ComputeMotion(g_scrollArrowsTime, 3, 11);
        auto scrollArrowAlphaMotionOutTime = ComputeMotion(g_scrollArrowsTime, 11, 4, true);
        auto scrollArrowAlphaMotionLoopTime = ComputeMotion(g_scrollArrowsTime, 15, 50);
        auto scrollArrowAlphaMotion = 255;

        if (scrollArrowAlphaMotionPauseTime >= 1.0)
        {
            // Fade out arrows.
            scrollArrowAlphaMotion = 255 * scrollArrowAlphaMotionOutTime;

            // Reset loop.
            if (scrollArrowAlphaMotionLoopTime >= 1.0)
                g_scrollArrowsTime = ImGui::GetTime();
        }
        else
        {
            // Fade in arrows.
            scrollArrowAlphaMotion = 255 * scrollArrowAlphaMotionInTime;
        }

        auto scrollArrowColourMotion = IM_COL32(255, 255, 255, scrollArrowAlphaMotion);

        ImVec2 scrollArrowTopMin = { max.x - scrollArrowScale - Scale(64, true) - g_pillarboxWidth, min.y + Scale(12, true) };
        ImVec2 scrollArrowTopMax = { scrollArrowTopMin.x + scrollArrowScale, scrollArrowTopMin.y + scrollArrowScale };
        ImVec2 scrollArrowBottomMin = { scrollArrowTopMin.x, max.y - scrollArrowScale - Scale(16, true) };
        ImVec2 scrollArrowBottomMax = { scrollArrowTopMax.x, scrollArrowBottomMin.y + scrollArrowScale };

        // Draw top arrow.
        AddImageFlipped(g_upTexMainMenu1.get(), scrollArrowTopMin, scrollArrowTopMax, GET_UV_COORDS(scrollArrowUVs), scrollArrowColourMotion, false, true);

        // Draw bottom arrow.
        drawList->AddImage(g_upTexMainMenu1.get(), scrollArrowBottomMin, scrollArrowBottomMax, GET_UV_COORDS(scrollArrowUVs), scrollArrowColourMotion);
    }
}

static void DrawContainer(ImVec2 min, ImVec2 max)
{
    auto* drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;

    auto containerUVs = PIXELS_TO_UV_COORDS(1024, 1024, 1, 6, 736, 432);
    auto containerWidth = Scale(716, true);
    auto containerHeight = Scale(412, true);
    auto containerOffsetX = Scale(73, true);
    auto containerOffsetY = Scale(136, true);

    ImVec2 containerMin = { max.x - containerWidth - containerOffsetX, min.y + containerOffsetY };
    ImVec2 containerMax = { containerMin.x + containerWidth, containerMin.y + containerHeight };

    auto containerAlphaMotionTime = ComputeMotion(g_stateTime, 0, 5);
    auto containerAlphaMotion = IM_COL32(255, 255, 255, 255 * containerAlphaMotionTime);

    SetGradient(containerMin, containerMax, containerAlphaMotion, containerAlphaMotion, IM_COL32_WHITE_TRANS, IM_COL32(255, 255, 255, 20 * containerAlphaMotionTime));
    drawList->AddImage(g_upTexMainMenu9.get(), containerMin, containerMax, GET_UV_COORDS(containerUVs));
    ResetGradient();

    auto optionsOffsetX = Scale(170, true);
    auto optionsOffsetY = Scale(23, true);

    DrawOptions({ containerMin.x + optionsOffsetX, containerMin.y + optionsOffsetY }, { res.x, containerMax.y - optionsOffsetY });
}

void OptionsMenu::Draw()
{
    // s_isVisible = GetAsyncKeyState(VK_F5);

    if (!s_isVisible)
        return;

    auto* drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;

    ImVec2 min = { g_pillarboxWidth, g_letterboxHeight };
    ImVec2 max = { res.x - g_pillarboxWidth, res.y - g_letterboxHeight };

    auto upIsHeld = false;
    auto downIsHeld = false;
    auto leftIsHeld = false;
    auto rightIsHeld = false;

    for (auto& spInputManager : App::s_pApp->m_pDoc->m_vspInputManager)
    {
        auto& rPadState = spInputManager->m_PadState;

        if (rPadState.IsDown(Sonicteam::SoX::Input::KeyState_DpadUp) || -rPadState.LeftStickVertical > 0.5f)
            upIsHeld = true;

        if (!g_upWasHeld && upIsHeld)
            g_up = true;

        if (rPadState.IsDown(Sonicteam::SoX::Input::KeyState_DpadDown) || -rPadState.LeftStickVertical < -0.5f)
            downIsHeld = true;

        if (!g_downWasHeld && downIsHeld)
            g_down = true;

        if (rPadState.IsDown(Sonicteam::SoX::Input::KeyState_DpadLeft) || -rPadState.LeftStickHorizontal > 0.5f)
            leftIsHeld = true;

        if (!g_leftWasHeld && leftIsHeld)
            g_left = true;

        if (rPadState.IsDown(Sonicteam::SoX::Input::KeyState_DpadRight) || -rPadState.LeftStickHorizontal < -0.5f)
            rightIsHeld = true;

        if (!g_rightWasHeld && rightIsHeld)
            g_right = true;

        if (rPadState.IsPressed(Sonicteam::SoX::Input::KeyState_A))
            g_isAccepted = true;

        if (rPadState.IsPressed(Sonicteam::SoX::Input::KeyState_B))
            g_isDeclined = true;

        if (rPadState.IsPressed(Sonicteam::SoX::Input::KeyState_X))
            g_isReset = true;
    }

    auto alphaMotionTime = s_isPause ? ComputeMotion(g_stateTime, 0, 10, s_state == OptionsMenuState::Closing) : 0.0;
    auto alpha = s_isPause ? Lerp(0, 175, alphaMotionTime) : 255;
    auto gradientTop = IM_COL32(0, 103, 255, alpha);
    auto gradientBottom = IM_COL32(0, 41, 100, alpha);

    drawList->AddRectFilledMultiColor({ 0.0f, g_letterboxHeight }, { res.x, res.y - g_letterboxHeight }, gradientTop, gradientTop, gradientBottom, gradientBottom);

    switch (s_state)
    {
        case OptionsMenuState::Closing:
        {
            s_commonMenu.Draw();

            if (s_commonMenu.Close(s_isPause))
                s_isVisible = false;

            break;
        }

        default:
        {
            if (s_state == OptionsMenuState::Opening)
            {
                s_commonMenu.Open();
                s_state = OptionsMenuState::Idle;
            }

            switch (s_flowState)
            {
                case OptionsMenuFlowState::CategoryCursor:
                {
                    auto categoryCount = (int)OptionsMenuCategory::Count;

                    // Remove codes category from cursor select.
                    if (!s_isCodesUnlocked)
                        categoryCount -= 1;

                    MoveCursor(g_categoryIndex, g_flowStateTime, 0, categoryCount, []()
                    {
                        ResetOptionSelection();

                        g_cursorArrowsTime = ImGui::GetTime();
                        s_commonMenu.SetDescription(GetCategoryDescription((OptionsMenuCategory)g_categoryIndex));
                    });

                    if (CheckAndDiscard(g_isAccepted))
                    {
                        Game_PlaySound("main_deside");

                        s_flowState = OptionsMenuFlowState::OptionCursor;
                        g_flowStateTime = ImGui::GetTime();
                        g_cursorArrowsTime = g_flowStateTime;
                        g_scrollArrowsTime = g_flowStateTime;
                    }

                    if (CheckAndDiscard(g_isDeclined))
                        Close();

                    break;
                }

                case OptionsMenuFlowState::OptionCursor:
                {
                    MoveCursor(g_optionIndex, g_flowStateTime, 0, g_optionCount, []()
                    {
                        g_cursorArrowsTime = ImGui::GetTime();
                    });

                    if (CheckAndDiscard(g_isDeclined))
                    {
                        Game_PlaySound("window_close");

                        s_flowState = OptionsMenuFlowState::CategoryCursor;
                        g_flowStateTime = ImGui::GetTime();
                        g_cursorArrowsTime = g_flowStateTime;

                        s_commonMenu.SetDescription(GetCategoryDescription((OptionsMenuCategory)g_categoryIndex));
                    }

                    break;
                }
            }

            DrawArrows({ 0, res.y / 2 - Scale(10) }, res);
            DrawCategories(min, max);
            DrawContainer(min, max);

            s_commonMenu.Draw();

            break;
        }
    }

    g_up = false;
    g_upWasHeld = upIsHeld;
    g_down = false;
    g_downWasHeld = downIsHeld;
    g_left = false;
    g_leftWasHeld = leftIsHeld;
    g_right = false;
    g_rightWasHeld = rightIsHeld;
    g_isAccepted = false;
    g_isDeclined = false;
    g_isReset = false;
}

void OptionsMenu::Init()
{
    g_upTexMainMenu7 = LOAD_ZSTD_TEXTURE(g_main_menu7);
    g_upTexMainMenu8 = LOAD_ZSTD_TEXTURE(g_main_menu8);
    g_upTexMainMenu9 = LOAD_ZSTD_TEXTURE(g_main_menu9);
}

void OptionsMenu::Open(bool isPause)
{
    s_commonMenu = CommonMenu(Localise("Options_Header_Name"), "", isPause);
    g_stateTime = ImGui::GetTime();
    g_cursorArrowsTime = g_stateTime;

    s_state = OptionsMenuState::Opening;
    s_isVisible = true;
    s_isPause = isPause;

    ResetSelection();
}

void OptionsMenu::Close()
{
    if (s_state == OptionsMenuState::Closing)
        return;

    s_state = OptionsMenuState::Closing;
    g_stateTime = ImGui::GetTime();
    g_cursorArrowsTime = g_stateTime;

    Game_PlaySound("window_close");
    Config::Save();
}

bool OptionsMenu::CanClose()
{
    return OptionsMenu::s_flowState == OptionsMenuFlowState::CategoryCursor;
}
