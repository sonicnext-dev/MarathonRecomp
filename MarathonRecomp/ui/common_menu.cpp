#include "common_menu.h"
#include <patches/aspect_ratio_patches.h>
#include <ui/imgui_utils.h>
#include <app.h>

static constexpr double ANIMATION_DURATION = 10.0;

void CommonMenu::Draw()
{
    auto* drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;

    if (App::s_isInit)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);

    ImVec2 min = { g_horzCentre, g_vertCentre };
    ImVec2 max = { res.x - min.x, res.y - min.y };

    auto borderMotionTime = PlayTransitions ? ComputeLinearMotion(m_time, 0, 10, m_isClosing) : 1.0;

    auto topPlateCornerUVs = PIXELS_TO_UV_COORDS(1024, 1024, 0, 154, 250, 144);
    auto topPlateLeftStretchUVs = PIXELS_TO_UV_COORDS(1024, 1024, 2, 154, 150, 144);
    auto topPlateRightStretchUVs = PIXELS_TO_UV_COORDS(1024, 1024, 250, 154, 750, 144);
    auto topPlateHeight = Scale(145, true);
    auto topPlateMotion = Lerp(min.y - topPlateHeight, min.y - Scale(0.2, true), borderMotionTime);

    ImVec2 topPlateCornerMin = { min.x - Scale(46, true), topPlateMotion };
    ImVec2 topPlateCornerMax = { topPlateCornerMin.x + Scale(250, true), topPlateMotion + topPlateHeight };
    ImVec2 topPlateStretchMin = { topPlateCornerMax.x, topPlateCornerMin.y };
    ImVec2 topPlateStretchMax = { res.x, topPlateCornerMax.y };

    auto redStripCornerUVs = PIXELS_TO_UV_COORDS(1024, 1024, 0, 301, 300, 50);
    auto redStripLeftStretchUVs = PIXELS_TO_UV_COORDS(1024, 1024, 0, 301, 150, 50);
    auto redStripRightStretchUVs = PIXELS_TO_UV_COORDS(1024, 1024, 300, 301, 300, 50);
    auto redStripColour = IM_COL32(168, 15, 15, 255);
    auto redStripOffsetY = Scale(81.5, true);
    auto redStripHeight = Scale(50, true);
    auto redStripMotion = topPlateMotion + redStripOffsetY;

    // TODO: account for metal plate height in motion.
    ImVec2 redStripCornerMin = { min.x - Scale(43.5, true), redStripMotion };
    ImVec2 redStripCornerMax = { redStripCornerMin.x + Scale(300, true), redStripMotion + redStripHeight };

    auto gradientTop = IM_COL32(0, 103, 255, 255);
    auto gradientBottom = IM_COL32(0, 92, 229, 255);
    auto gradientHeight = Scale(120, true);

    // Draw gradient to fill gap between red strip and top metal plates.
    if (!ReduceDraw)
        drawList->AddRectFilledMultiColor({ 0.0f, topPlateMotion }, { res.x, topPlateMotion + gradientHeight }, gradientTop, gradientTop, gradientBottom, gradientBottom);

    if (!ReduceDraw)
    {
        // Draw corner left red strip.
        drawList->AddImage(g_upTexMainMenu1.get(), redStripCornerMin, redStripCornerMax, GET_UV_COORDS(redStripCornerUVs), redStripColour);

        // Draw left stretched red strip.
        drawList->AddImage(g_upTexMainMenu1.get(), { 0.0f, redStripCornerMin.y }, { redStripCornerMin.x, redStripCornerMax.y }, GET_UV_COORDS(redStripLeftStretchUVs), redStripColour);

        // Draw right stretched red strip.
        drawList->AddImage(g_upTexMainMenu1.get(), { redStripCornerMax.x, redStripCornerMin.y }, { res.x, redStripCornerMax.y }, GET_UV_COORDS(redStripRightStretchUVs), redStripColour);

        auto redStripHighlightWidth = Scale(270, true);
        auto redStripHighlightHeight = Scale(48, true);
        auto redStripHighlightY = redStripCornerMin.y - Scale(3, true);

        auto drawRedStripHighlight = [&](ImVec2 redStripHighlightMin, uint32_t tlColour, uint32_t brColour)
        {
            auto redStripHighlightUVs = PIXELS_TO_UV_COORDS(1024, 1024, 201, 501, 264, 50);

            ImVec2 redStripHighlightMax = { redStripHighlightMin.x + redStripHighlightWidth, redStripHighlightMin.y + redStripHighlightHeight };

            SetAdditive(true);
            SetHorizontalGradient(redStripHighlightMin, redStripHighlightMax, tlColour, brColour);

            drawList->AddImage(g_upTexMainMenu1.get(), redStripHighlightMin, redStripHighlightMax, GET_UV_COORDS(redStripHighlightUVs));

            ResetGradient();
            ResetAdditive();
        };

        // Draw fixed highlights for red strip.
        drawRedStripHighlight({ redStripCornerMax.x + Scale(68, true), redStripHighlightY }, IM_COL32(255, 172, 0, 0), IM_COL32(255, 172, 0, 67));
        drawRedStripHighlight({ redStripCornerMax.x + Scale(-43.5, true), redStripHighlightY }, IM_COL32(255, 89, 0, 0), IM_COL32(255, 89, 0, 20));

        auto redStripHighlightMotionOffsetX = Scale(63, true);
        auto redStripHighlightMotionX1Time = ComputeLinearMotion(m_titleTime, PlayTransitions ? 10 : 0, 10, m_isClosing);
        auto redStripHighlightMotionX2Time = ComputeLinearMotion(m_titleTime, PlayTransitions ? 12 : 2, 10, m_isClosing);
        auto redStripHighlightMotionAlphaIn1Time = ComputeLinearMotion(m_titleTime, PlayTransitions ? 10 : 0, 8, m_isClosing);
        auto redStripHighlightMotionAlphaIn2Time = ComputeLinearMotion(m_titleTime, PlayTransitions ? 12 : 2, 8, m_isClosing);
        auto redStripHighlightMotionAlphaOut1Time = ComputeLinearMotion(m_titleTime, PlayTransitions ? 18 : 8, 2, m_isClosing);
        auto redStripHighlightMotionAlphaOut2Time = ComputeLinearMotion(m_titleTime, PlayTransitions ? 20 : 2, 2, m_isClosing);
        auto redStripHighlightMotionX1 = Lerp(res.x + redStripHighlightWidth, min.x + redStripHighlightMotionOffsetX, redStripHighlightMotionX1Time);
        auto redStripHighlightMotionX2 = Lerp(res.x + redStripHighlightWidth, min.x + redStripHighlightMotionOffsetX, redStripHighlightMotionX2Time);
        auto redStripHighlightMotionAlpha1 = Lerp(0.0, 100.0, redStripHighlightMotionAlphaIn1Time);
        auto redStripHighlightMotionAlpha2 = Lerp(0.0, 100.0, redStripHighlightMotionAlphaIn2Time);

        if (redStripHighlightMotionAlphaIn1Time >= 1.0)
            redStripHighlightMotionAlpha1 = Lerp(100.0, 0.0, redStripHighlightMotionAlphaOut1Time);

        if (redStripHighlightMotionAlphaIn2Time >= 1.0)
            redStripHighlightMotionAlpha2 = Lerp(100.0, 0.0, redStripHighlightMotionAlphaOut2Time);

        // Draw animated highlights for title animation.
        drawRedStripHighlight({ redStripHighlightMotionX1, redStripHighlightY }, IM_COL32(255, 172, 0, 0), IM_COL32(255, 172, 0, redStripHighlightMotionAlpha1));
        drawRedStripHighlight({ redStripHighlightMotionX2, redStripHighlightY }, IM_COL32(255, 172, 0, 0), IM_COL32(255, 172, 0, redStripHighlightMotionAlpha2));
    }

    auto titleText = Title.empty() ? "DUMMY" : Title.data();
    auto titleFontSize = Scale(33, true);
    auto titleSize = g_pFntNewRodin->CalcTextSizeA(titleFontSize, FLT_MAX, 0.0f, titleText);
    auto titleOffsetX = redStripCornerMax.x - Scale(105, true);
    auto titleOffsetY = redStripMotion + Scale(3.5, true);
    auto titleMotionTime = ComputeLinearMotion(m_titleTime, PlayTransitions && !m_isClosing ? 10 : 0, 10, m_isClosing);
    auto titleOffsetXMotion = Lerp(max.x + titleSize.x, titleOffsetX, titleMotionTime);

    if (!ReduceDraw && !m_previousTitle.empty())
    {
        auto prevTitleAlphaMotionTime = ComputeLinearMotion(m_titleTime, PlayTransitions ? 10 : 0, 3, m_isClosing);

        // Draw previous title fading out.
        drawList->AddText(g_pFntNewRodin, titleFontSize, { titleOffsetX, titleOffsetY }, IM_COL32(255, 255, 255, Lerp(255, 0, prevTitleAlphaMotionTime)), m_previousTitle.data());
    }

    // Draw title.
    drawList->AddText(g_pFntNewRodin, titleFontSize, { titleOffsetXMotion, titleOffsetY }, IM_COL32(255, 255, 255, 255 * titleMotionTime), titleText);

    if (!ReduceDraw)
    {
        // Draw top left corner metal plate.
        drawList->AddImage(g_upTexMainMenu1.get(), topPlateCornerMin, topPlateCornerMax, GET_UV_COORDS(topPlateCornerUVs));

        // Draw top right stretched metal plate.
        SetHorizontalGradient(topPlateStretchMin, topPlateStretchMax, IM_COL32_WHITE, IM_COL32(200, 200, 200, 255));
        drawList->AddImage(g_upTexMainMenu1.get(), topPlateStretchMin, topPlateStretchMax, GET_UV_COORDS(topPlateRightStretchUVs));
        ResetGradient();

        // Draw top left stretched metal plate for ultrawide.
        if (g_aspectRatio > WIDE_ASPECT_RATIO)
            AddImageFlipped(g_upTexMainMenu1.get(), { 0.0f, topPlateCornerMin.y }, { topPlateCornerMin.x + Scale(2, true), topPlateCornerMax.y }, GET_UV_COORDS(topPlateLeftStretchUVs), IM_COL32_WHITE, true);

        // Draw flipped metal plates for narrow aspect ratios.
        if (g_aspectRatio < WIDE_ASPECT_RATIO)
        {
            ImVec2 topPlateCornerExtendMin = { topPlateCornerMin.x, topPlateCornerMin.y - topPlateHeight };
            ImVec2 topPlateCornerExtendMax = { topPlateCornerMax.x, topPlateCornerMin.y + Scale(1, true) };
            ImVec2 topPlateStretchExtendMin = { topPlateStretchMin.x, topPlateStretchMin.y - topPlateHeight };
            ImVec2 topPlateStretchExtendMax = { topPlateStretchMax.x, topPlateStretchMin.y + Scale(1, true) };

            AddImageFlipped(g_upTexMainMenu1.get(), topPlateCornerExtendMin, topPlateCornerExtendMax, GET_UV_COORDS(topPlateCornerUVs), IM_COL32_WHITE, false, true);
            SetHorizontalGradient(topPlateStretchExtendMin, topPlateStretchExtendMax, IM_COL32_WHITE, IM_COL32(200, 200, 200, 255));
            AddImageFlipped(g_upTexMainMenu1.get(), topPlateStretchExtendMin, topPlateStretchExtendMax, GET_UV_COORDS(topPlateRightStretchUVs), IM_COL32_WHITE, false, true);
            ResetGradient();
        }
    }

    if (ShowDescription)
    {
        auto textCoverCornerUVs = PIXELS_TO_UV_COORDS(1024, 1024, 801, 400, 150, 150);
        auto textCoverCornerExtendUVs = PIXELS_TO_UV_COORDS(1024, 1024, 801, 400, 125, 150);
        auto textCoverCentreUVs = PIXELS_TO_UV_COORDS(1024, 1024, 950, 400, 50, 150);
        auto textCoverCornerUVCompensation = Scale(2, true);
        auto textCoverOffsetY = Scale(16.4, true);
        auto textCoverWidth = Scale(149.5, true);
        auto textCoverHeight = Scale(150, true);
        auto textCoverMotion = Lerp(max.y + textCoverHeight, max.y - textCoverOffsetY - textCoverHeight, borderMotionTime);
        auto textCoverColour = IM_COL32(0, 23, 57, 255);

        ImVec2 textCoverCornerLeftMin = { min.x, textCoverMotion };
        ImVec2 textCoverCornerLeftMax = { textCoverCornerLeftMin.x + textCoverWidth, textCoverCornerLeftMin.y + textCoverHeight };
        ImVec2 textCoverCentreMin = { textCoverCornerLeftMax.x, textCoverCornerLeftMin.y };
        ImVec2 textCoverCentreMax = { max.x - textCoverWidth, textCoverCornerLeftMax.y };
        ImVec2 textCoverCornerRightMin = { max.x - textCoverWidth, textCoverCornerLeftMin.y };
        ImVec2 textCoverCornerRightMax = { max.x, textCoverCornerLeftMax.y };

        if (ReduceDraw)
        {
            auto horzMargin = Scale(128, true);

            ImVec2 textCoverClipMin = { textCoverCornerLeftMin.x + horzMargin, textCoverCornerLeftMin.y + Scale(14, true) };
            ImVec2 textCoverClipMax = { textCoverCornerRightMax.x - horzMargin, textCoverCornerRightMax.y - Scale(90, true) };

            drawList->PushClipRect(textCoverClipMin, textCoverClipMax);
        }

        if (!Description.empty())
        {
            auto descFadeScale = Scale(20, true);
            auto descFontSize = Scale(Config::Language == ELanguage::Japanese ? 28 : 27, true);
            auto descSize = g_pFntRodin->CalcTextSizeA(descFontSize, FLT_MAX, 0.0f, Description.data());

            ImVec2 descBoundsMin = { textCoverCentreMin.x - Scale(18, true), textCoverCentreMin.y + Scale(20, true) };
            ImVec2 descBoundsMax = { textCoverCentreMax.x + Scale(18, true), textCoverCentreMax.y - Scale(90, true) };
            auto descBoundsWidth = descBoundsMax.x - descBoundsMin.x;

            m_descPos =
            {
                descBoundsMin.x + ((descBoundsMax.x - descBoundsMin.x) / 2) - (descSize.x / 2),
                descBoundsMin.y + ((descBoundsMax.y - descBoundsMin.y) / 2) - (descSize.y / 2)
            };

            if (descSize.x > descBoundsWidth)
            {
                auto descScrollMax = descSize.x - (descBoundsWidth - descFadeScale * 2);
                auto descScrollSpeed = Scale(150, true);
                auto descScrollDelay = 1.2f;

                if (descScrollMax > 0.0f)
                {
                    auto horz = -m_inputListener.RightStickX;

                    if (fabs(horz) > 0.25f)
                    {
                        m_isDescManualScrolling = true;
                        m_descScrollOffset += horz * descScrollSpeed * App::s_deltaTime;
                    }
                    else if (m_isDescManualScrolling && fabs(horz) <= 0.25f)
                    {
                        m_isDescScrolling = false;
                        m_isDescManualScrolling = false;
                        m_descScrollTimer = 0.0f;
                        m_descScrollDirection = horz > 0.0f ? 1.0f : -1.0f;
                    }

                    if (!m_isDescManualScrolling)
                    {
                        if (!m_isDescScrolling)
                        {
                            m_descScrollTimer += App::s_deltaTime;

                            if (m_descScrollTimer >= descScrollDelay)
                                m_isDescScrolling = true;
                        }

                        if (m_isDescScrolling)
                        {
                            m_descScrollOffset += descScrollSpeed * m_descScrollDirection * App::s_deltaTime;

                            if (m_descScrollOffset >= descScrollMax)
                            {
                                m_isDescScrolling = false;
                                m_descScrollOffset = descScrollMax;
                                m_descScrollTimer = 0.0f;
                                m_descScrollDirection = -1.0f;
                            }
                            else if (m_descScrollOffset <= 0.0f)
                            {
                                m_isDescScrolling = false;
                                m_descScrollOffset = 0;
                                m_descScrollTimer = 0.0f;
                                m_descScrollDirection = 1.0f;
                            }
                        }
                    }

                    m_descScrollOffset = std::clamp(m_descScrollOffset, 0.0f, descScrollMax);
                }
                else
                {
                    m_isDescScrolling = false;
                    m_descScrollOffset = 0.0f;
                    m_descScrollTimer = 0.0f;
                    m_descScrollDirection = 1.0f;
                }

                m_descPos.x = (descBoundsMin.x + descFadeScale) - m_descScrollOffset;
            }

            auto descAlphaMotionTime = ComputeLinearMotion(m_descTime, PlayTransitions ? 10 : 0, 15, m_isClosing);

            // Draw text cover backdrop.
            drawList->AddRectFilled({ 0.0f, textCoverMotion }, { res.x, textCoverMotion + textCoverHeight }, IM_COL32(0, 0, 0, 65));

            // Draw previous description fading out.
            if (!m_isClosing && !m_previousDesc.empty())
                drawList->AddText(g_pFntRodin, descFontSize, m_previousDescPos, IM_COL32(255, 255, 255, Lerp(255, 0, descAlphaMotionTime)), m_previousDesc.data());

            // Draw description.
            drawList->AddText(g_pFntRodin, descFontSize, m_descPos, IM_COL32(255, 255, 255, Lerp(0, 255, descAlphaMotionTime)), Description.data());

            // Draw left text cover.
            drawList->AddImage(g_upTexMainMenu1.get(), { 0.0f, textCoverCornerLeftMin.y }, { textCoverCornerLeftMin.x + textCoverCornerUVCompensation, textCoverCornerLeftMax.y }, GET_UV_COORDS(textCoverCornerExtendUVs), textCoverColour);
            drawList->AddImage(g_upTexMainMenu1.get(), textCoverCornerLeftMin, textCoverCornerLeftMax, GET_UV_COORDS(textCoverCornerUVs), textCoverColour);

            // Draw centre text cover.
            drawList->AddImage(g_upTexMainMenu1.get(), textCoverCentreMin, textCoverCentreMax, GET_UV_COORDS(textCoverCentreUVs), textCoverColour);

            // Draw right text cover.
            AddImageFlipped(g_upTexMainMenu1.get(), { textCoverCornerRightMax.x - textCoverCornerUVCompensation, textCoverCornerRightMin.y }, { res.x, textCoverCornerRightMax.y }, GET_UV_COORDS(textCoverCornerExtendUVs), textCoverColour);
            AddImageFlipped(g_upTexMainMenu1.get(), textCoverCornerRightMin, textCoverCornerRightMax, GET_UV_COORDS(textCoverCornerUVs), textCoverColour, true);
        }
        else
        {
            // Draw blank text cover.
            drawList->AddRectFilled({ 0.0f, textCoverCornerLeftMin.y }, { res.x, textCoverCornerLeftMax.y }, textCoverColour);
        }
    }

    if (ReduceDraw)
    {
        drawList->PopClipRect();
    }
    else
    {
        auto bottomPlateUVs = PIXELS_TO_UV_COORDS(1024, 1024, 1, -17, 700, 145);
        auto bottomPlateStretchUVs = PIXELS_TO_UV_COORDS(1024, 1024, 1, -17, 128, 145);
        auto bottomPlateOffsetX = Scale(-60, true);
        auto bottomPlateOffsetY = Scale(12, true);
        auto bottomPlateWidth = Scale(700, true);
        auto bottomPlateHeight = Scale(145, true);
        auto bottomPlateMotion = Lerp(max.y + bottomPlateHeight, (max.y + bottomPlateOffsetY) - bottomPlateHeight, borderMotionTime);

        ImVec2 bottomPlateLeftMin = { min.x + bottomPlateOffsetX, bottomPlateMotion };
        ImVec2 bottomPlateLeftMax = { bottomPlateLeftMin.x + bottomPlateWidth, bottomPlateMotion + bottomPlateHeight };
        ImVec2 bottomPlateRightMin = { max.x - bottomPlateWidth - bottomPlateOffsetX, bottomPlateLeftMin.y };
        ImVec2 bottomPlateRightMax = { bottomPlateRightMin.x + bottomPlateWidth, bottomPlateLeftMax.y };
        
        // Draw bottom left metal plate.
        drawList->AddImage(g_upTexMainMenu1.get(), bottomPlateLeftMin, bottomPlateLeftMax, GET_UV_COORDS(bottomPlateUVs));

        // Draw bottom right metal plate.
        AddImageFlipped(g_upTexMainMenu1.get(), bottomPlateRightMin, bottomPlateRightMax, GET_UV_COORDS(bottomPlateUVs), IM_COL32_WHITE, true);

        // Draw stretched metal plates for ultrawide.
        if (g_aspectRatio > WIDE_ASPECT_RATIO)
        {
            // Draw bottom left stretched metal plate.
            AddImageFlipped(g_upTexMainMenu1.get(), { 0.0f, bottomPlateLeftMin.y }, { bottomPlateLeftMin.x, bottomPlateLeftMax.y }, GET_UV_COORDS(bottomPlateStretchUVs), IM_COL32_WHITE, true);

            // Draw bottom right stretched metal plate.
            AddImageFlipped(g_upTexMainMenu1.get(), { bottomPlateRightMax.x, bottomPlateRightMin.y }, { res.x, bottomPlateRightMax.y }, GET_UV_COORDS(bottomPlateStretchUVs), IM_COL32_WHITE, true);
        }

        // Draw flipped metal plates for narrow aspect ratios.
        if (g_aspectRatio < WIDE_ASPECT_RATIO)
        {
            ImVec2 bottomPlateLeftExtendMin = { bottomPlateLeftMin.x, bottomPlateLeftMax.y };
            ImVec2 bottomPlateLeftExtendMax = { bottomPlateLeftMax.x, bottomPlateRightMax.y + bottomPlateHeight };
            ImVec2 bottomPlateRightExtendMin = { bottomPlateRightMin.x, bottomPlateRightMax.y };
            ImVec2 bottomPlateRightExtendMax = { bottomPlateRightMax.x, bottomPlateRightMax.y + bottomPlateHeight };

            AddImageFlipped(g_upTexMainMenu1.get(), bottomPlateLeftExtendMin, bottomPlateLeftExtendMax, GET_UV_COORDS(bottomPlateUVs), IM_COL32_WHITE, false, true);
            AddImageFlipped(g_upTexMainMenu1.get(), bottomPlateRightExtendMin, bottomPlateRightExtendMax, GET_UV_COORDS(bottomPlateUVs), IM_COL32_WHITE, true, true);
        }
    }

    if (App::s_isInit)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);

    if (ShowVersionString)
    {
        auto verAlphaMotionTime = PlayTransitions ? ComputeLinearMotion(m_time, 0, m_isClosing ? 3 : 10, m_isClosing) : 1.0;

        DrawVersionString(IM_COL32(0, 0, 0, 70 * verAlphaMotionTime));
    }
}

void CommonMenu::Open()
{
    m_isClosing = false;
    m_time = ImGui::GetTime();
    m_titleTime = m_time;
    m_descTime = m_time;
}

double CommonMenu::Close(bool isAnimated)
{
    if (!m_isClosing)
    {
        m_isClosing = true;
        m_time = ImGui::GetTime();
        m_titleTime = m_time;
        m_descTime = m_time;
    }

    return isAnimated ? ComputeLinearMotion(m_time, 0, ANIMATION_DURATION) : 1.0;
}

bool CommonMenu::IsOpen() const
{
    return !m_isClosing && ComputeLinearMotion(m_time, 0, ANIMATION_DURATION) >= 1.0;
}

void CommonMenu::SetTitle(std::string title, bool isAnimated)
{
    if (Title == title)
        return;

    m_previousTitle = Title;
    Title = title;

    if (!isAnimated)
        return;

    m_titleTime = ImGui::GetTime();
}

void CommonMenu::SetDescription(std::string desc, bool isAnimated)
{
    if (Description == desc)
        return;

    m_previousDesc = Description;
    m_previousDescPos = m_descPos;
    Description = desc;

    resetDescScroll();

    if (!isAnimated)
        return;

    m_descTime = ImGui::GetTime();
}
