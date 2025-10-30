#include "imgui_utils.h"
#include <gpu/imgui/imgui_snapshot.h>
#include <hid/hid.h>
#include <patches/aspect_ratio_patches.h>
#include <ui/black_bar.h>
#include <app.h>
#include <decompressor.h>
#include <version.h>

#include <res/images/common/button_window.dds.h>
#include <res/images/common/controller.dds.h>
#include <res/images/common/kbm.dds.h>
#include <res/images/common/window.dds.h>
#include <res/images/common/select_arrow.dds.h>
#include <res/images/common/main_menu1.dds.h>
#include <res/images/common/arrow.dds.h>

ImFont* g_pFntRodin;
ImFont* g_pFntNewRodin;

std::unique_ptr<GuestTexture> g_upTexButtonWindow;
std::unique_ptr<GuestTexture> g_upTexController;
std::unique_ptr<GuestTexture> g_upTexKbm;
std::unique_ptr<GuestTexture> g_upTexWindow;
std::unique_ptr<GuestTexture> g_upTexSelectArrow;
std::unique_ptr<GuestTexture> g_upTexMainMenu1;
std::unique_ptr<GuestTexture> g_upTexArrow;

void InitImGuiUtils()
{
    g_pFntRodin = ImFontAtlasSnapshot::GetFont("FOT-RodinPro-DB.otf");
    g_pFntNewRodin = ImFontAtlasSnapshot::GetFont("FOT-NewRodinPro-UB.otf");

    g_upTexButtonWindow = LOAD_ZSTD_TEXTURE(g_button_window);
    g_upTexController = LOAD_ZSTD_TEXTURE(g_controller);
    g_upTexKbm = LOAD_ZSTD_TEXTURE(g_kbm);
    g_upTexWindow = LOAD_ZSTD_TEXTURE(g_window);
    g_upTexSelectArrow = LOAD_ZSTD_TEXTURE(g_select_arrow);
    g_upTexMainMenu1 = LOAD_ZSTD_TEXTURE(g_main_menu1);
    g_upTexArrow = LOAD_ZSTD_TEXTURE(g_arrow);
}

void SetGradient(const ImVec2& min, const ImVec2& max, ImU32 top, ImU32 bottom)
{
    SetGradient(min, max, top, top, bottom, bottom);
}

void SetHorizontalGradient(const ImVec2& min, const ImVec2& max, ImU32 left, ImU32 right)
{
    SetGradient(min, max, left, right, right, left);
}

void SetVerticalGradient(const ImVec2& min, const ImVec2& max, ImU32 top, ImU32 bottom)
{
    SetGradient(min, max, top, top, bottom, bottom);
}

void SetGradient(const ImVec2& min, const ImVec2& max, ImU32 topLeft, ImU32 topRight, ImU32 bottomRight, ImU32 bottomLeft)
{
    auto callbackData = AddImGuiCallback(ImGuiCallback::SetGradient);
    callbackData->setGradient.boundsMin[0] = min.x;
    callbackData->setGradient.boundsMin[1] = min.y;
    callbackData->setGradient.boundsMax[0] = max.x;
    callbackData->setGradient.boundsMax[1] = max.y;
    callbackData->setGradient.gradientTopLeft = topLeft;
    callbackData->setGradient.gradientTopRight = topRight;
    callbackData->setGradient.gradientBottomRight = bottomRight;
    callbackData->setGradient.gradientBottomLeft = bottomLeft;
}

void ResetGradient()
{
    auto callbackData = AddImGuiCallback(ImGuiCallback::SetGradient);
    memset(&callbackData->setGradient, 0, sizeof(callbackData->setGradient));
}

void SetShaderModifier(uint32_t shaderModifier)
{
    if (shaderModifier == IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT && Config::DisableLowResolutionFontOnCustomUI)
        shaderModifier = IMGUI_SHADER_MODIFIER_NONE;

    auto callbackData = AddImGuiCallback(ImGuiCallback::SetShaderModifier);
    callbackData->setShaderModifier.shaderModifier = shaderModifier;
}

void SetOrigin(ImVec2 origin)
{
    auto callbackData = AddImGuiCallback(ImGuiCallback::SetOrigin);
    callbackData->setOrigin.origin[0] = origin.x;
    callbackData->setOrigin.origin[1] = origin.y;
}

void SetScale(ImVec2 scale)
{
    auto callbackData = AddImGuiCallback(ImGuiCallback::SetScale);
    callbackData->setScale.scale[0] = scale.x;
    callbackData->setScale.scale[1] = scale.y;
}

void SetTextSkew(float yCenter, float skewScale)
{
    SetShaderModifier(IMGUI_SHADER_MODIFIER_TEXT_SKEW);
    SetOrigin({ 0.0f, yCenter });
    SetScale({ skewScale, 1.0f });
}

void ResetTextSkew()
{
    SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);
    SetOrigin({ 0.0f, 0.0f });
    SetScale({ 1.0f, 1.0f });
}

void SetHorizontalMarqueeFade(ImVec2 min, ImVec2 max, float fadeScaleLeft, float fadeScaleRight)
{
    auto callbackData = AddImGuiCallback(ImGuiCallback::SetMarqueeFade);
    callbackData->setMarqueeFade.boundsMin[0] = min.x;
    callbackData->setMarqueeFade.boundsMin[1] = min.y;
    callbackData->setMarqueeFade.boundsMax[0] = max.x;
    callbackData->setMarqueeFade.boundsMax[1] = max.y;

    SetShaderModifier(IMGUI_SHADER_MODIFIER_HORIZONTAL_MARQUEE_FADE);
    SetScale({ fadeScaleLeft, fadeScaleRight });
}

void SetHorizontalMarqueeFade(ImVec2 min, ImVec2 max, float fadeScale)
{
    SetHorizontalMarqueeFade(min, max, fadeScale, fadeScale);
}

void SetVerticalMarqueeFade(ImVec2 min, ImVec2 max, float fadeScaleTop, float fadeScaleBottom)
{
    auto callbackData = AddImGuiCallback(ImGuiCallback::SetMarqueeFade);
    callbackData->setMarqueeFade.boundsMin[0] = min.x;
    callbackData->setMarqueeFade.boundsMin[1] = min.y;
    callbackData->setMarqueeFade.boundsMax[0] = max.x;
    callbackData->setMarqueeFade.boundsMax[1] = max.y;

    SetShaderModifier(IMGUI_SHADER_MODIFIER_VERTICAL_MARQUEE_FADE);
    SetScale({ fadeScaleTop, fadeScaleBottom });
}

void SetVerticalMarqueeFade(ImVec2 min, ImVec2 max, float fadeScale)
{
    SetVerticalMarqueeFade(min, max, fadeScale, fadeScale);
}

void ResetMarqueeFade()
{
    ResetGradient();
    SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);
    SetScale({ 1.0f, 1.0f });
}

void SetOutline(float outline)
{
    auto callbackData = AddImGuiCallback(ImGuiCallback::SetOutline);
    callbackData->setOutline.outline = outline;
}

void ResetOutline()
{
    SetOutline(0.0f);
}

void SetProceduralOrigin(ImVec2 proceduralOrigin)
{
    auto callbackData = AddImGuiCallback(ImGuiCallback::SetProceduralOrigin);
    callbackData->setProceduralOrigin.proceduralOrigin[0] = proceduralOrigin.x;
    callbackData->setProceduralOrigin.proceduralOrigin[1] = proceduralOrigin.y;
}

void ResetProceduralOrigin()
{
    SetProceduralOrigin({ 0.0f, 0.0f });
}

void SetAdditive(bool enabled)
{
    auto callbackData = AddImGuiCallback(ImGuiCallback::SetAdditive);
    callbackData->setAdditive.enabled = enabled;
}

void ResetAdditive()
{
    SetAdditive(false);
}

void AddImageFlipped(ImTextureID texture, const ImVec2& min, const ImVec2& max, const ImVec2& uvMin, const ImVec2& uvMax, ImU32 col, bool flipHorz, bool flipVert)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    ImVec2 uv0 = uvMin;
    ImVec2 uv1 = { uvMax.x, uvMin.y };
    ImVec2 uv2 = uvMax;
    ImVec2 uv3 = { uvMin.x, uvMax.y };

    if (flipHorz)
    {
        std::swap(uv0.x, uv1.x);
        std::swap(uv2.x, uv3.x);
    }

    if (flipVert)
    {
        std::swap(uv0.y, uv3.y);
        std::swap(uv1.y, uv2.y);
    }

    ImVec2 p0 = min;
    ImVec2 p1 = { max.x, min.y };
    ImVec2 p2 = max;
    ImVec2 p3 = { min.x, max.y };

    drawList->AddImageQuad(texture, p0, p1, p2, p3, uv0, uv1, uv2, uv3, col);
}

float Scale(float size, bool useGameplayScale)
{
    auto result = size * g_aspectRatioScale;

    if (useGameplayScale)
        result *= g_aspectRatioGameplayScale;

    return result;
}

double ComputeLoopMotion(double time, double offset, double total)
{
    return std::clamp(fmod((ImGui::GetTime() - time - (offset / 60.0)) / (total / 60.0), 1.0 + (total / 60.0)), 0.0, 1.0) / 1.0;
}

double ComputeLinearMotion(double time, double offset, double total, bool reverse)
{
    auto result = std::clamp((ImGui::GetTime() - time - offset / 60.0) / total * 60.0, 0.0, 1.0);

    return reverse ? 1.0f - result : result;
}

double ComputeMotion(double time, double offset, double total, bool reverse)
{
    return sqrt(ComputeLinearMotion(time, offset, total, reverse));
}

void DrawArrows(ImVec2 min, ImVec2 max, double& time)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    constexpr auto FADE_IN_DURATION = 5.0;
    constexpr auto FADE_OUT_DURATION = 121.0;
    constexpr auto ARROWS_OFFSET_FRAMES = 3.0;

    auto arrowUVs = PIXELS_TO_UV_COORDS(512, 512, 0, 0, 400, 434);
    auto centre = ImVec2(min.x + ((max.x - min.x) / 2), min.y + ((max.y - min.y) / 2));

    auto leftArrowWidth = Scale(400, true);
    auto leftArrowHeight = Scale(434, true);
    auto leftArrowNextOffset = Scale(230, true);

    auto rightArrowWidth = Scale(200, true);
    auto rightArrowHeight = Scale(217, true);
    auto rightArrowNextOffset = Scale(150, true);

    const auto leftArrowCount = int((max.x + leftArrowWidth) / leftArrowNextOffset) + 1;
    const auto rightArrowCount = int((max.x + rightArrowWidth) / rightArrowNextOffset) + 1;
    const auto fadeOutStartTime = (ARROWS_OFFSET_FRAMES * rightArrowCount) + (FADE_IN_DURATION * rightArrowCount) + 30.0;

    auto computeArrowLoopMotion = [&](double offset) -> double
    {
        auto motionTime = 0.0;
        auto fadeOutStartMotionTime = ComputeMotion(time, 0, fadeOutStartTime);

        if (fadeOutStartMotionTime < 1.0)
        {
            motionTime = ComputeMotion(time, offset, FADE_IN_DURATION);
        }
        else
        {
            motionTime = ComputeMotion(time, fadeOutStartTime, FADE_OUT_DURATION, true);

            if (motionTime <= 0.0)
                time = ImGui::GetTime();
        }

        return std::clamp(motionTime, 0.0, 1.0);
    };

    auto leftArrowsOffsetFrames = ARROWS_OFFSET_FRAMES * (rightArrowCount - 1);
    auto animDuration = ((FADE_IN_DURATION + FADE_OUT_DURATION) + (leftArrowsOffsetFrames + (ARROWS_OFFSET_FRAMES * leftArrowCount))) + fadeOutStartTime;

    auto leftArrowOffsetMotionTime = ComputeMotion(time, 0, animDuration);
    auto leftArrowGlobalOffset = Scale(Lerp(318, 298, leftArrowOffsetMotionTime), true);

    auto rightArrowOffsetMotionTime = ComputeMotion(time, 0, animDuration - 20.0);
    auto rightArrowGlobalOffset = Scale(Lerp(73, 103, rightArrowOffsetMotionTime), true);

    auto rightBaseY = centre.y - (rightArrowHeight / 2) - Scale(19, true);
    auto rightEndY = rightBaseY + rightArrowHeight;
    
    for (int i = 0; i < rightArrowCount; i++)
    {
        auto baseX = max.x - (i * rightArrowNextOffset) - rightArrowWidth + rightArrowGlobalOffset;
        auto endX = baseX + rightArrowWidth;
        auto opacity = Lerp(5, 45, std::clamp(baseX / max.x, 0.0f, 1.0f)) * computeArrowLoopMotion(ARROWS_OFFSET_FRAMES * (rightArrowCount - i));

        drawList->AddImage(g_upTexArrow.get(), { baseX, rightBaseY }, { endX, rightEndY }, GET_UV_COORDS(arrowUVs), IM_COL32(255, 255, 255, opacity));
    }

    auto leftBaseY = centre.y - (leftArrowHeight / 2) - Scale(20, true);
    auto leftEndY = leftBaseY + leftArrowHeight;

    for (int i = 0; i < leftArrowCount; i++)
    {
        auto baseX = min.x + (i * leftArrowNextOffset) - leftArrowWidth + leftArrowGlobalOffset;
        auto endX = baseX + leftArrowWidth;
        auto opacity = Lerp(45, 5, std::clamp(baseX / max.x, 0.0f, 1.0f)) * computeArrowLoopMotion(leftArrowsOffsetFrames + (ARROWS_OFFSET_FRAMES * (leftArrowCount - i)));

        drawList->AddImage(g_upTexArrow.get(), { endX, leftBaseY }, { baseX, leftEndY }, GET_UV_COORDS(arrowUVs), IM_COL32(255, 255, 255, opacity));
    }
}

void DrawArrowCursor(ImVec2 pos, double time, bool isIntroAnim, bool isBlinkingAnim, bool isReversed)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    auto cursorUVs = PIXELS_TO_UV_COORDS(50, 50, 0, 0, 27, 50);
    auto cursorScaleX = Scale(14, true);

    for (int i = 0; i < 3; i++)
    {
        auto cursorMotionTime = isIntroAnim ? ComputeLinearMotion(time, i, 5, isReversed) : 1.0;
        auto cursorScaleYMotion = Lerp(Scale(37.5, true), Scale(24, true), cursorMotionTime);
        auto cursorOffsetXMotion = Lerp(0, Scale(8.5, true), cursorMotionTime);
        auto cursorOffsetYMotion = Lerp(pos.y - cursorScaleYMotion / 6, pos.y, cursorMotionTime);
        auto cursorRightMotion = (cursorOffsetXMotion * 3) - (cursorOffsetXMotion * i);
        auto cursorAlphaMotionTime = isIntroAnim ? ComputeLinearMotion(time, i, 2, isReversed) : 1.0;
        auto cursorAlphaMotion = (int)Lerp(0, 255, cursorAlphaMotionTime);

        if (isBlinkingAnim)
        {
            auto cursorAlphaMotionInTime = ComputeLoopMotion(time, 3.0 * i, 12.0);
            auto cursorAlphaMotionOutTime = ComputeLoopMotion(time, 3.0 * (i + 1), 12.0);

            // horrible
            cursorAlphaMotionTime = cursorAlphaMotionInTime >= 1.0
                ? cursorAlphaMotionOutTime >= 1.0
                    ? cursorAlphaMotionInTime
                    : cursorAlphaMotionOutTime
                : cursorAlphaMotionInTime;

            cursorAlphaMotion = (int)Lerp(50 * (i + 1), 255, cursorAlphaMotionTime);
        }

        ImVec2 cursorMin = { pos.x + cursorRightMotion, cursorOffsetYMotion };
        ImVec2 cursorMax = { pos.x + cursorRightMotion + cursorScaleX, cursorMin.y + cursorScaleYMotion };

        drawList->AddImage(g_upTexSelectArrow.get(), cursorMin, cursorMax, GET_UV_COORDS(cursorUVs), IM_COL32(255, 255, 255, cursorAlphaMotion));
    }
}

double ImValueDebug(double& value, double increment)
{
#ifdef _WIN32
    if (GetAsyncKeyState(VK_OEM_PLUS) & 1)
        value += increment;
    if (GetAsyncKeyState(VK_OEM_MINUS) & 1)
        value -= increment;
#endif

    LOGF_UTILITY("{}", value);

    return value;
}

void DrawContainerBox(ImVec2 min, ImVec2 max, float alpha)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    auto commonWidth = Scale(50);
    auto commonHeight = Scale(50);
    auto bottomHeight = Scale(5);

    auto tl = PIXELS_TO_UV_COORDS(1024, 1024, 0, 400, 50, 50);
    auto tc = PIXELS_TO_UV_COORDS(1024, 1024, 50, 400, 50, 50);
    auto cl = PIXELS_TO_UV_COORDS(1024, 1024, 0, 450, 50, 50);
    auto cc = PIXELS_TO_UV_COORDS(1024, 1024, 50, 450, 50, 50);
    auto bl = PIXELS_TO_UV_COORDS(1024, 1024, 0, 500, 50, 50);
    auto bc = PIXELS_TO_UV_COORDS(1024, 1024, 50, 500, 50, 50);

    auto color = IM_COL32(255, 255, 255, 100 * alpha);

    SetHorizontalGradient({ max.x - commonWidth, min.y }, max, IM_COL32_WHITE, IM_COL32(255, 255, 255, 0));

    drawList->AddImage(g_upTexMainMenu1.get(), min, { min.x + commonWidth, min.y + commonHeight }, GET_UV_COORDS(tl), color);
    drawList->AddImage(g_upTexMainMenu1.get(), { min.x + commonWidth, min.y }, { max.x, min.y + commonHeight }, GET_UV_COORDS(tc), color);
    drawList->AddImage(g_upTexMainMenu1.get(), { min.x, min.y + commonHeight }, { min.x + commonWidth, max.y - commonHeight }, GET_UV_COORDS(cl), color);
    drawList->AddImage(g_upTexMainMenu1.get(), { min.x + commonWidth, min.y + commonHeight }, { max.x, max.y - commonHeight }, GET_UV_COORDS(cc), color);
    drawList->AddImage(g_upTexMainMenu1.get(), { min.x, max.y - commonHeight }, { min.x + commonWidth, max.y + bottomHeight }, GET_UV_COORDS(bl), color);
    drawList->AddImage(g_upTexMainMenu1.get(), { min.x + commonWidth, max.y - commonHeight }, { max.x, max.y + bottomHeight }, GET_UV_COORDS(bc), color);

    ResetGradient();
}

void DrawTextBasic(const ImFont* font, float fontSize, const ImVec2& pos, ImU32 colour, const char* text)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    drawList->AddText(font, fontSize, pos, colour, text, nullptr);
}

void DrawTextWithMarquee(const ImFont* font, float fontSize, const ImVec2& position, const ImVec2& min, const ImVec2& max, ImU32 color, const char* text, double time, double delay, double speed)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    auto rectWidth = max.x - min.x;
    auto textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, text);
    auto textX = position.x - fmodf(std::max(0.0, ImGui::GetTime() - (time + delay)) * speed, textSize.x + rectWidth);

    drawList->PushClipRect(min, max, true);

    if (textX <= position.x)
        DrawTextBasic(font, fontSize, { textX, position.y }, color, text);

    if (textX + textSize.x < position.x)
        DrawTextBasic(font, fontSize, { textX + textSize.x + rectWidth, position.y }, color, text);

    drawList->PopClipRect();
}

void DrawTextWithMarqueeShadow(const ImFont* font, float fontSize, const ImVec2& pos, const ImVec2& min, const ImVec2& max, ImU32 colour, const char* text, double time, double delay, double speed, float offset, float radius, ImU32 shadowColour)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    auto rectWidth = max.x - min.x;
    auto textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, text);
    auto textX = pos.x - fmodf(std::max(0.0, ImGui::GetTime() - (time + delay)) * speed, textSize.x + rectWidth);

    drawList->PushClipRect(min, max, true);

    if (textX <= pos.x)
        DrawTextWithShadow(font, fontSize, { textX, pos.y }, colour, text, offset, radius, shadowColour);

    if (textX + textSize.x < pos.x)
        DrawTextWithShadow(font, fontSize, { textX + textSize.x + rectWidth, pos.y }, colour, text, offset, radius, shadowColour);

    drawList->PopClipRect();
}

void DrawTextWithOutline(const ImFont* font, float fontSize, const ImVec2& pos, ImU32 color, const char* text, float outlineSize, ImU32 outlineColor, uint32_t shaderModifier)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    SetOutline(outlineSize);
    drawList->AddText(font, fontSize, pos, outlineColor, text);
    ResetOutline();

    if (shaderModifier != IMGUI_SHADER_MODIFIER_NONE)
        SetShaderModifier(shaderModifier);

    drawList->AddText(font, fontSize, pos, color, text);

    if (shaderModifier != IMGUI_SHADER_MODIFIER_NONE)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);
}

void DrawTextWithShadow(const ImFont* font, float fontSize, const ImVec2& pos, ImU32 colour, const char* text, float offset, float radius, ImU32 shadowColour)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    offset = Scale(offset);

    SetOutline(radius);
    drawList->AddText(font, fontSize, { pos.x + offset, pos.y + offset }, shadowColour, text);
    ResetOutline();

    drawList->AddText(font, fontSize, pos, colour, text, nullptr);
}

void DrawTextParagraph(const ImFont* font, float fontSize, float maxWidth, const ImVec2& pos, float lineMargin, const char* text, std::function<void(const char*, ImVec2)> drawMethod, bool isCentred)
{
    auto lines = Split(text, font, fontSize, maxWidth);
    auto paragraphSize = MeasureCentredParagraph(font, fontSize, lineMargin, lines);
    auto offsetY = 0.0f;

    for (auto& line : lines)
    {
        auto textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, line.c_str());
        auto textX = pos.x;
        auto textY = pos.y + offsetY;

        if (isCentred)
        {
            textX = line.starts_with("- ")
                ? pos.x - paragraphSize.x / 2
                : pos.x - textSize.x / 2;

            textY = pos.y - paragraphSize.y / 2 + offsetY;
        }

        drawMethod(line.c_str(), { textX, textY });

        textX += textSize.x;
        offsetY += textSize.y + Scale(lineMargin);
    }
}

float CalcWidestTextSize(const ImFont* font, float fontSize, std::span<std::string> strs)
{
    auto result = 0.0f;

    for (auto& str : strs)
        result = std::max(result, font->CalcTextSizeA(fontSize, FLT_MAX, 0, str.c_str()).x);

    return result;
}

std::string Truncate(const std::string& input, size_t maxLength, bool useEllipsis, bool usePrefixEllipsis)
{
    const std::string ellipsis = "...";

    if (input.length() > maxLength)
    {
        if (useEllipsis && maxLength > ellipsis.length())
        {
            if (usePrefixEllipsis)
            {
                return ellipsis + input.substr(0, maxLength - ellipsis.length());
            }
            else
            {
                return input.substr(0, maxLength - ellipsis.length()) + ellipsis;
            }
        }
        else
        {
            return input.substr(0, maxLength);
        }
    }

    return input;
}

std::vector<std::string> Split(const char* strStart, const ImFont* font, float fontSize, float maxWidth)
{
    if (!strStart)
        return {};

    std::vector<std::string> result;
    float textWidth = 0.0f;
    float lineWidth = 0.0f;
    const float scale = fontSize / font->FontSize;
    const char *str = strStart;
    const char *strEnd = strStart + strlen(strStart);
    const char *lineStart = strStart;
    const bool wordWrapEnabled = (maxWidth > 0.0f);
    const char *wordWrapEOL = nullptr;

    auto IsKanji = [](const char* str, const char* strEnd)
    {
        const char* tempStr = str;
        unsigned int c = (unsigned int)*tempStr;
        if (c < 0x80)
            tempStr += 1;
        else
            tempStr += ImTextCharFromUtf8(&c, tempStr, strEnd);

        // Basic CJK and CJK Extension A
        return (c >= 0x4E00 && c <= 0x9FBF) || (c >= 0x3400 && c <= 0x4DBF);
    };

    while (*str != 0) 
    {
        if (wordWrapEnabled)
        {
            if (wordWrapEOL == nullptr)
            {
                wordWrapEOL = CalcWordWrapPositionA(font, scale, str, strEnd, maxWidth - lineWidth);
            }

            if (str >= wordWrapEOL)
            {
                if (IsKanji(str, strEnd))
                {
                    // If the current character is Kanji, move back to prevent splitting Kanji
                    while (str > lineStart && IsKanji(str - 3, strEnd))
                    {
                        str -= 3;
                    }
                }

                if (textWidth < lineWidth)
                    textWidth = lineWidth;

                result.emplace_back(lineStart, str);
                lineWidth = 0.0f;
                wordWrapEOL = nullptr;

                while (str < strEnd && ImCharIsBlankA(*str))
                    str++;

                if (*str == '\n')
                    str++;

                if (strncmp(str, "\u200B", 3) == 0)
                {
                    str += 3;
                }

                lineStart = str;
                continue;
            }
        }

        const char *prevStr = str;
        unsigned int c = (unsigned int)*str;
        if (c < 0x80)
            str += 1;
        else
            str += ImTextCharFromUtf8(&c, str, strEnd);

        if (c < 32)
        {
            if (c == '\n')
            {
                result.emplace_back(lineStart, str - 1);
                lineStart = str;
                textWidth = ImMax(textWidth, lineWidth);
                lineWidth = 0.0f;
                continue;
            }

            if (c == '\r')
            {
                lineStart = str;
                continue;
            }
        }

        const float charWidth = ((int)c < font->IndexAdvanceX.Size ? font->IndexAdvanceX.Data[c] : font->FallbackAdvanceX) * scale;
        if (lineWidth + charWidth >= maxWidth)
        {
            str = prevStr;
            break;
        }

        lineWidth += charWidth;
    }

    if (str != lineStart) 
    {
        result.emplace_back(lineStart, str);
    }

    return result;
}

ImVec2 MeasureCentredParagraph(const ImFont* font, float fontSize, float lineMargin, const std::vector<std::string>& lines)
{
    auto x = 0.0f;
    auto y = 0.0f;

    for (size_t i = 0; i < lines.size(); i++)
    {
        auto textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, lines[i].c_str());

        x = std::max(x, textSize.x);
        y += textSize.y + Scale(lineMargin);
    }

    return { x, y };
}

ImVec2 MeasureCentredParagraph(const ImFont* font, float fontSize, float maxWidth, float lineMargin, const char* text)
{
    return MeasureCentredParagraph(font, fontSize, lineMargin, Split(text, font, fontSize, maxWidth));
}

float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

float Cubic(float a, float b, float t)
{
    return a + (b - a) * (t * t * t);
}

float Hermite(float a, float b, float t)
{
    return a + (b - a) * (t * t * (3 - 2 * t));
}

ImVec2 Lerp(const ImVec2& a, const ImVec2& b, float t)
{
    return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
}

ImU32 ColourLerp(ImU32 c0, ImU32 c1, float t)
{
    auto a = ImGui::ColorConvertU32ToFloat4(c0);
    auto b = ImGui::ColorConvertU32ToFloat4(c1);

    ImVec4 result;
    result.x = a.x + (b.x - a.x) * t;
    result.y = a.y + (b.y - a.y) * t;
    result.z = a.z + (b.z - a.z) * t;
    result.w = a.w + (b.w - a.w) * t;

    return ImGui::ColorConvertFloat4ToU32(result);
}

void DrawVersionString(const ImU32 colour)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;

    auto fontSize = Scale(12, true);
    auto textSize = g_pFntNewRodin->CalcTextSizeA(fontSize, FLT_MAX, 0, g_versionString);

    auto textMargin = Scale(2, true);
    auto textY = res.y - textSize.y - textMargin;

    if (g_aspectRatio < NARROW_ASPECT_RATIO)
        textY -= BlackBar::s_letterboxHeight - BlackBar::s_margin;

    // TODO: remove this line after v1 release.
    drawList->AddText(g_pFntNewRodin, fontSize, { textMargin, textY }, colour, "WORK IN PROGRESS");
    drawList->AddText(g_pFntNewRodin, fontSize, { res.x - textSize.x - textMargin, textY }, colour, g_versionString);
}

// Taken from ImGui because we need to modify to break for '\u200B\ too
// Simple word-wrapping for English, not full-featured. Please submit failing cases!
// This will return the next location to wrap from. If no wrapping if necessary, this will fast-forward to e.g. text_end.
// FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within "word,,,,", more sensible support for punctuations, support for Unicode punctuations, etc.)
const char* CalcWordWrapPositionA(const ImFont* font, float scale, const char* text, const char* text_end, float wrap_width)
{
    // For references, possible wrap point marked with ^
    //  "aaa bbb, ccc,ddd. eee   fff. ggg!"
    //      ^    ^    ^   ^   ^__    ^    ^

    // List of hardcoded separators: .,;!?'"

    // Skip extra blanks after a line returns (that includes not counting them in width computation)
    // e.g. "Hello    world" --> "Hello" "World"

    // Cut words that cannot possibly fit within one line.
    // e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"
    float line_width = 0.0f;
    float word_width = 0.0f;
    float blank_width = 0.0f;
    wrap_width /= scale; // We work with unscaled widths to avoid scaling every characters

    const char* word_end = text;
    const char* prev_word_end = NULL;
    bool inside_word = true;

    const char* s = text;
    IM_ASSERT(text_end != NULL);
    while (s < text_end)
    {
        unsigned int c = (unsigned int)*s;
        const char* next_s;
        if (c < 0x80)
            next_s = s + 1;
        else
            next_s = s + ImTextCharFromUtf8(&c, s, text_end);

        if (c < 32)
        {
            if (c == '\n')
            {
                line_width = word_width = blank_width = 0.0f;
                inside_word = true;
                s = next_s;
                continue;
            }
            if (c == '\r')
            {
                s = next_s;
                continue;
            }
        }

        const float char_width = ((int)c < font->IndexAdvanceX.Size ? font->IndexAdvanceX.Data[c] : font->FallbackAdvanceX);
        if (ImCharIsBlankW(c) || c == 0x200B)
        {
            if (inside_word)
            {
                line_width += blank_width;
                blank_width = 0.0f;
                word_end = s;
            }
            blank_width += char_width;
            inside_word = false;
        }
        else
        {
            word_width += char_width;
            if (inside_word)
            {
                word_end = next_s;
            }
            else
            {
                prev_word_end = word_end;
                line_width += word_width + blank_width;
                word_width = blank_width = 0.0f;
            }

            // Allow wrapping after punctuation.
            inside_word = (c != '.' && c != ',' && c != ';' && c != '!' && c != '?' && c != '\"');
        }

        // We ignore blank width at the end of the line (they can be skipped)
        if (line_width + word_width > wrap_width)
        {
            // Words that cannot possibly fit within an entire line will be cut anywhere.
            if (word_width < wrap_width)
                s = prev_word_end ? prev_word_end : word_end;
            break;
        }

        s = next_s;
    }

    // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
    // +1 may not be a character start point in UTF-8 but it's ok because caller loops use (text >= word_wrap_eol).
    if (s == text && text < text_end)
        return s + 1;

    return s;
}

static std::vector<std::string> ParseInterpolatedString(const char* str, bool includeTokens = true)
{
    std::vector<std::string> result;

    // Skip nullptr or strings that cannot possibly
    // contain variables (e.g. "${}" bare minimum).
    if (!str || strlen(str) <= 3)
        return result;

    auto start = str;
    auto ptr = str;

    while (*ptr)
    {
        if (ptr[0] == '$' && ptr[1] == '{')
        {
            // Add leading text.
            if (ptr > start)
                result.emplace_back(start, ptr - start);

            auto tokenStart = ptr;

            // Skip "${".
            ptr += 2;

            // Seek to end token.
            while (*ptr && *ptr != '}')
                ++ptr;

            if (*ptr == '}')
            {
                auto curStrStart = tokenStart;
                auto curStrEnd = ++ptr;

                if (!includeTokens)
                {
                    curStrStart += 2;
                    --curStrEnd;
                }

                // Add variable.
                result.emplace_back(curStrStart, curStrEnd - curStrStart);
                start = ptr;
            }
            else
            {
                break;
            }
        }
        else
        {
            ++ptr;
        }
    }

    // Add trailing text.
    if (ptr > start)
        result.emplace_back(start, ptr - start);

    return result;
}

static bool IsInterpolatedString(std::string_view str)
{
    return str.length() >= 3 && str[0] == '$' && str[1] == '{' && str.back() == '}';
}

ImVec2 MeasureInterpolatedText(const ImFont* pFont, float fontSize, const char* pText, ImGuiTextInterpData* pInterpData)
{
    ImVec2 result{};

    auto parsed = ParseInterpolatedString(pText);

    auto measureText = [&](float paddingX, std::string_view str)
    {
        auto textSize = pFont->CalcTextSizeA(fontSize, FLT_MAX, 0, str.data());

        result.x += textSize.x + paddingX;
        result.y = std::max(result.y, textSize.y);
    };

    for (size_t i = 0; i < parsed.size(); i++)
    {
        auto& str = parsed[i];
        auto  paddingX = i == parsed.size() - 1 ? 0 : Scale(2, true);

        if (IsInterpolatedString(str))
        {
            auto parsedSingleNoTokens = ParseInterpolatedString(str.data(), false);

            if (!parsedSingleNoTokens.size())
                continue;

            auto variableMapSingle = MapTextVariables(parsedSingleNoTokens[0].data());

            if (!variableMapSingle.size())
                continue;

            auto& variable = variableMapSingle[0];
            auto& variableType = variable.first;
            auto& variableValue = variable.second;

            if (variableType == "picture")
            {
                auto& pictureName = variableValue;
                auto  pictureNameHash = HashStr(pictureName);

                if (!pInterpData || !pInterpData->Picture.pupTexture || !pInterpData->pPictureCrops || !pInterpData->pPictureCrops->contains(pictureNameHash))
                {
                    auto placeholderScale = Scale(28, true);

                    result.x += placeholderScale;
                    result.y = std::max(result.y, placeholderScale);

                    continue;
                }

                auto pictureCrop = FindHash<ImGuiTextPictureCrop>(*pInterpData->pPictureCrops, pictureNameHash);
                auto pictureWidth = Scale(pictureCrop.Width, true);
                auto pictureHeight = Scale(pictureCrop.Height, true);

                result.x += pictureWidth + paddingX;
                result.y = std::max(result.y, pictureHeight);
            }
            else if (variableType == "locale")
            {
                auto& localeName = variableValue;

                measureText(paddingX, Localise(localeName));
            }
        }
        else
        {
            measureText(paddingX, str);
        }
    }

    return result;
}

void DrawInterpolatedText(const ImFont* pFont, float fontSize, const ImVec2& pos, ImU32 colour, const char* pText, ImGuiTextInterpData* pInterpData)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    auto parsed = ParseInterpolatedString(pText);
    auto advanceX = 0.0f;
    auto marginY = Config::Language == ELanguage::Japanese ? 0 : Scale(1, true);

    auto drawText = [&](const ImVec2& pos, float paddingX, std::string_view str)
    {
        auto textSize = pFont->CalcTextSizeA(fontSize, FLT_MAX, 0, str.data());

        drawList->AddText(pFont, fontSize, pos, colour, str.data());

        advanceX += textSize.x + paddingX;
    };

    for (size_t i = 0; i < parsed.size(); i++)
    {
        auto& str = parsed[i];
        auto  curPos = ImVec2{ pos.x + advanceX, pos.y + marginY };
        auto  paddingX = i == parsed.size() - 1 ? 0 : Scale(2, true);

        if (IsInterpolatedString(str))
        {
            auto parsedSingleNoTokens = ParseInterpolatedString(str.data(), false);
        
            if (!parsedSingleNoTokens.size())
                continue;

            auto variableMapSingle = MapTextVariables(parsedSingleNoTokens[0].data());
        
            if (!variableMapSingle.size())
                continue;
        
            auto& variable = variableMapSingle[0];
            auto& variableType = variable.first;
            auto& variableValue = variable.second;
        
            if (variableType == "picture")
            {
                auto& pictureName = variableValue;
                auto  pictureNameHash = HashStr(pictureName);

                if (!pInterpData || !pInterpData->Picture.pupTexture || !pInterpData->pPictureCrops || !pInterpData->pPictureCrops->contains(pictureNameHash))
                {
                    auto placeholderScale = Scale(28, true);

                    ImVec2 placeholderMin = { pos.x + advanceX, pos.y };
                    ImVec2 placeholderMax = { placeholderMin.x + placeholderScale, placeholderMin.y + placeholderScale };

                    advanceX += placeholderScale;

                    drawList->AddRectFilled(placeholderMin, placeholderMax, IM_COL32(255, 0, 0, 255));

                    continue;
                }

                auto pictureCrop = FindHash<ImGuiTextPictureCrop>(*pInterpData->pPictureCrops, pictureNameHash);
                auto pictureUVs = PIXELS_TO_UV_COORDS(pInterpData->Picture.Width, pInterpData->Picture.Height, pictureCrop.X, pictureCrop.Y, pictureCrop.Width, pictureCrop.Height);
                auto pictureWidth = Scale(pictureCrop.Width, true);
                auto pictureHeight = Scale(pictureCrop.Height, true);

                ImVec2 pictureMin = { pos.x + advanceX, pos.y };
                ImVec2 pictureMax = { pictureMin.x + pictureWidth, pictureMin.y + pictureHeight };

                advanceX += pictureWidth + paddingX;

                drawList->AddImage(pInterpData->Picture.pupTexture->get(), pictureMin, pictureMax, GET_UV_COORDS(pictureUVs), colour);
            }
            else if (variableType == "locale")
            {
                auto& localeName = variableValue;

                drawText(curPos, paddingX, Localise(localeName));
            }
        }
        else
        {
            drawText(curPos, paddingX, str);
        }
    }
}

ImGuiTextInterpData GetHidInterpTextData()
{
    auto buttonTexture = &g_upTexController;
    auto buttonTextureWidth = uint16_t(256);
    auto buttonTextureHeight = uint16_t(128);

    auto buttonCrops = Config::IsControllerIconsPS3() 
        ? &g_buttonCropsPS3
        : &g_buttonCropsXenon;

    if (!App::s_isInit)
    {
        if (hid::g_inputDevice == hid::EInputDevice::Keyboard ||
            hid::g_inputDevice == hid::EInputDevice::Mouse)
        {
            buttonTexture = &g_upTexKbm;
            buttonTextureWidth = uint16_t(384);
        }

        if (hid::g_inputDevice == hid::EInputDevice::Keyboard)
        {
            buttonCrops = &g_buttonCropsKeyboard;
        }
        else if (hid::g_inputDevice == hid::EInputDevice::Mouse)
        {
            buttonCrops = &g_buttonCropsMouse;
        }
    }

    return { { buttonTexture, buttonTextureWidth, buttonTextureHeight }, buttonCrops };
}

const xxHashMap<ImGuiTextPictureCrop> g_buttonCropsXenon =
{
    { HashStr("button_a"),     { 0, 0, 28, 28 } },
    { HashStr("button_b"),     { 28, 0, 28, 28 } },
    { HashStr("button_x"),     { 56, 0, 28, 28 } },
    { HashStr("button_y"),     { 84, 0, 28, 28 } },
    { HashStr("button_lb"),    { 112, 0, 53, 28 } },
    { HashStr("button_rb"),    { 168, 0, 53, 28 } },
    { HashStr("button_lt"),    { 56, 29, 55, 28 } },
    { HashStr("button_rt"),    { 0, 29, 55, 28 } },
    { HashStr("button_start"), { 112, 28, 28, 28 } },
    { HashStr("button_back"),  { 140, 28, 28, 28 } }
};

const xxHashMap<ImGuiTextPictureCrop> g_buttonCropsPS3 =
{
    { HashStr("button_a"),     { 0, 56, 28, 28 } },
    { HashStr("button_b"),     { 28, 56, 28, 28 } },
    { HashStr("button_x"),     { 56, 56, 28, 28 } },
    { HashStr("button_y"),     { 84, 56, 28, 28 } },
    { HashStr("button_lb"),    { 112, 56, 48, 28 } },
    { HashStr("button_rb"),    { 0, 84, 48, 28 } },
    { HashStr("button_lt"),    { 168, 56, 48, 28 } },
    { HashStr("button_rt"),    { 56, 84, 48, 28 } },
    { HashStr("button_start"), { 140, 84, 28, 28 } },
    { HashStr("button_back"),  { 112, 84, 28, 28 } }
};

const xxHashMap<ImGuiTextPictureCrop> g_buttonCropsMouse =
{
    { HashStr("button_a"), { 128, 0, 128, 128 } },
    { HashStr("button_b"), { 256, 0, 128, 128 } }
};

const xxHashMap<ImGuiTextPictureCrop> g_buttonCropsKeyboard =
{
    { HashStr("button_a"), { 0, 0, 128, 128 } },
    { HashStr("button_b"), { 256, 0, 128, 128 } }
};
