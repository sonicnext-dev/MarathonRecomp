#include "imgui_utils.h"
#include <patches/aspect_ratio_patches.h>
#include <app.h>
#include <decompressor.h>
#include <version.h>

#include <res/images/common/general_window.dds.h>
#include <res/images/common/light.dds.h>
#include <res/images/common/select.dds.h>
#include <res/images/common/main_menu1.dds.h>
#include <res/images/common/arrow.dds.h>

std::unique_ptr<GuestTexture> g_texGeneralWindow;
std::unique_ptr<GuestTexture> g_texLight;
std::unique_ptr<GuestTexture> g_texSelect;
std::unique_ptr<GuestTexture> g_texMainMenu1;
std::unique_ptr<GuestTexture> g_texArrow;

void InitImGuiUtils()
{
    g_texGeneralWindow = LOAD_ZSTD_TEXTURE(g_general_window);
    g_texLight = LOAD_ZSTD_TEXTURE(g_light);
    g_texSelect = LOAD_ZSTD_TEXTURE(g_select);
    g_texMainMenu1 = LOAD_ZSTD_TEXTURE(g_main_menu1);
    g_texArrow = LOAD_ZSTD_TEXTURE(arrow);
}

void SetGradient(const ImVec2& min, const ImVec2& max, ImU32 top, ImU32 bottom)
{
    SetGradient(min, max, top, top, bottom, bottom);
}

void SetHorizontalGradient(const ImVec2& min, const ImVec2& max, ImU32 left, ImU32 right)
{
    SetGradient(min, max, left, right, right, left);
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

float Scale(float size)
{
    return size * g_aspectRatioScale;
}

static double ComputeLoopMotion(double time, double offset)
{
    const double fadeInDuration = 0.1;
    const double fadeOutDuration = 10.0;

    double elapsedTime = time - offset;

    if (elapsedTime < 0.0) {
        return 0.0;
    }

    double result;

    if (elapsedTime < fadeInDuration) {
        double progress = elapsedTime / fadeInDuration;
        result = 1.0 - pow(1.0 - progress, 2.0);

    } else if (elapsedTime < fadeInDuration + fadeOutDuration) {
        double fadeOutTime = elapsedTime - fadeInDuration;
        double progress = fadeOutTime / fadeOutDuration;
        result = pow(1.0 - progress, 10.0);

    } else {
        result = 0.0;
    }

    return std::clamp(result, 0.0, 1.0);
}

double ComputeLinearMotion(double duration, double offset, double total)
{
    return std::clamp((ImGui::GetTime() - duration - offset / 60.0) / total * 60.0, 0.0, 1.0);
}

double ComputeMotion(double duration, double offset, double total)
{
    return sqrt(ComputeLinearMotion(duration, offset, total));
}

void DrawArrows(ImVec2 min, ImVec2 max)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    const float maxOpacity = 0.15f;

    auto leftArrowSize = Scale(450);
    auto leftArrowOffset = Scale(230);
    auto rightArrowSize = Scale(180);
    auto rightArrowOffset = Scale(120);
    auto xOffset = Scale(10);

    auto arrowUV = PIXELS_TO_UV_COORDS(512, 512, 0, 0, 500, 434);

    auto elapsedTime = ImGui::GetTime();
    const double totalTime = 5.0;

    double cycleTime = fmod(elapsedTime, totalTime);
    auto totalAnimOffset = (float)(xOffset * totalTime);

    const uint32_t leftArrows = (uint32_t)((max.x + Scale(200) + totalAnimOffset) / leftArrowOffset) + 1;
    const uint32_t rightArrows = (uint32_t)((max.x + Scale(20) + totalAnimOffset) / rightArrowOffset) + 1;

    auto leftBaseY = min.y - (leftArrowSize / 2);
    auto leftEndY = min.y + (leftArrowSize / 2);

    for (uint32_t i = 0; i < leftArrows; i++)
    {
        auto baseX = min.x + (i * leftArrowOffset) - Scale(200) - totalAnimOffset;
        auto endX = baseX + leftArrowSize;
        auto opacity = (uint32_t)((maxOpacity * ComputeLoopMotion(cycleTime, 1.0 + ((double)(leftArrows - i) / leftArrows))) * 255);

        drawList->AddImage(g_texArrow.get(), { endX, leftBaseY }, { baseX, leftEndY }, GET_UV_COORDS(arrowUV), IM_COL32(255, 255, 255, opacity));
    }

    auto rightBaseY = min.y - (rightArrowSize / 2);
    auto rightEndY = min.y + (rightArrowSize / 2);

    for (uint32_t i = 0; i < rightArrows; i++)
    {
        auto baseX = max.x - (i * rightArrowOffset) - rightArrowSize + Scale(20) + totalAnimOffset;
        auto endX = baseX + rightArrowSize;
        auto opacity = (uint32_t)((maxOpacity * ComputeLoopMotion(cycleTime, ((double)(rightArrows - i) / rightArrows))) * 255);

        drawList->AddImage(g_texArrow.get(), { baseX, rightBaseY }, { endX, rightEndY }, GET_UV_COORDS(arrowUV), IM_COL32(255, 255, 255, opacity));
    }
}

void DrawHUD(ImVec2 min, ImVec2 max, const ImFont* font, const char* text)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    auto leftWidth = Scale(200);
    auto stripeHeight = Scale(50);
    auto offset = Scale(81);

    auto leftStripe = PIXELS_TO_UV_COORDS(1024, 1024, 0, 300, 200, 50);
    auto centerStripe = PIXELS_TO_UV_COORDS(1024, 1024, 200, 300, 824, 50);

    auto stripeColor = IM_COL32(168, 15, 15, 255);

    drawList->AddImage(g_texMainMenu1.get(), { min.x, min.y + offset }, { min.x + leftWidth, min.y + offset + stripeHeight }, GET_UV_COORDS(leftStripe), stripeColor);
    drawList->AddImage(g_texMainMenu1.get(), { min.x + leftWidth , min.y + offset }, { max.x, min.y + offset + stripeHeight }, GET_UV_COORDS(centerStripe), stripeColor);

    auto stripOffset = Scale(78);
    auto stripUV = PIXELS_TO_UV_COORDS(1024, 1024, 201, 501, 264, 50);

    auto strip1LeftOffset = leftWidth + Scale(180);

    auto tlStrip1Color = IM_COL32(255, 172, 0, 0);
    auto brStrip1Color = IM_COL32(255, 172, 0, 67);

    ImVec2 strip1Min = { min.x + strip1LeftOffset, min.y + stripOffset };
    ImVec2 strip1Max = { min.x + strip1LeftOffset + Scale(270), min.y + stripOffset + stripeHeight };

    SetHorizontalGradient(strip1Min, strip1Max, tlStrip1Color, brStrip1Color);

    drawList->AddImage(g_texMainMenu1.get(), strip1Min, strip1Max, GET_UV_COORDS(stripUV), IM_COL32_WHITE);

    ResetGradient();

    auto strip2LeftOffset = leftWidth + Scale(40);

    auto tlStrip2Color = IM_COL32(255, 116, 0, 0);
    auto brStrip2Color = IM_COL32(255, 116, 0, 17);

    ImVec2 strip2Min = { min.x + strip2LeftOffset, min.y + stripOffset };
    ImVec2 strip2Max = { min.x + strip2LeftOffset + Scale(270), min.y + stripOffset + stripeHeight };

    SetHorizontalGradient(strip2Min, strip2Max, tlStrip2Color, brStrip2Color);

    drawList->AddImage(g_texMainMenu1.get(), strip2Min, strip2Max, GET_UV_COORDS(stripUV), IM_COL32_WHITE);

    ResetGradient();

    auto backBarHeight = Scale(150);
    auto backBarColor = IM_COL32(0, 23, 57, 255);

    drawList->AddRectFilled({ min.x, max.y - backBarHeight }, { max.x, max.y }, backBarColor);

    auto leftSilver = PIXELS_TO_UV_COORDS(1024, 1024, 0, 154, 200, 116);
    auto centerSilver = PIXELS_TO_UV_COORDS(1024, 1024, 200, 154, 824, 116);

    auto silverHeight = Scale(116);

    drawList->AddImage(g_texMainMenu1.get(), { min.x - Scale(1), min.y }, { min.x - Scale(1) + leftWidth, min.y + silverHeight }, GET_UV_COORDS(leftSilver), IM_COL32_WHITE);
    drawList->AddImage(g_texMainMenu1.get(), { min.x - Scale(1) + leftWidth, min.y }, { max.x, min.y + silverHeight }, GET_UV_COORDS(centerSilver), IM_COL32_WHITE);

    auto centerBottomBar = PIXELS_TO_UV_COORDS(1024, 1024, 180, 60, 844, 55);

    auto bottomBarHeight = Scale(55);
    auto bottomTabWidth = Scale(180);

    drawList->AddImage(g_texMainMenu1.get(), { min.x + bottomTabWidth, max.y - bottomBarHeight }, { max.x - bottomTabWidth, max.y }, GET_UV_COORDS(centerBottomBar), IM_COL32_WHITE);

    auto bottomTab = PIXELS_TO_UV_COORDS(1024, 1024, 0, 0, 180, 115);

    auto bottomTabHeight = Scale(115);

    drawList->AddImage(g_texMainMenu1.get(), { min.x, max.y - bottomTabHeight }, { min.x + bottomTabWidth, max.y }, GET_UV_COORDS(bottomTab), IM_COL32_WHITE);
    drawList->AddImage(g_texMainMenu1.get(), { max.x, max.y - bottomTabHeight }, { max.x - bottomTabWidth, max.y }, GET_UV_COORDS(bottomTab), IM_COL32_WHITE);

    drawList->AddText(font, Scale(32), { min.x + leftWidth + Scale(2), min.y + offset + Scale(5) }, IM_COL32_WHITE, text);
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

    drawList->AddImage(g_texMainMenu1.get(), min, { min.x + commonWidth, min.y + commonHeight }, GET_UV_COORDS(tl), color);
    drawList->AddImage(g_texMainMenu1.get(), { min.x + commonWidth, min.y }, { max.x, min.y + commonHeight }, GET_UV_COORDS(tc), color);
    drawList->AddImage(g_texMainMenu1.get(), { min.x, min.y + commonHeight }, { min.x + commonWidth, max.y - commonHeight }, GET_UV_COORDS(cl), color);
    drawList->AddImage(g_texMainMenu1.get(), { min.x + commonWidth, min.y + commonHeight }, { max.x, max.y - commonHeight }, GET_UV_COORDS(cc), color);
    drawList->AddImage(g_texMainMenu1.get(), { min.x, max.y - commonHeight }, { min.x + commonWidth, max.y + bottomHeight }, GET_UV_COORDS(bl), color);
    drawList->AddImage(g_texMainMenu1.get(), { min.x + commonWidth, max.y - commonHeight }, { max.x, max.y + bottomHeight }, GET_UV_COORDS(bc), color);

    ResetGradient();
}

void DrawPauseContainer(ImVec2 min, ImVec2 max, float alpha)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    auto commonWidth = Scale(35);
    auto commonHeight = Scale(35);
    auto bottomHeight = Scale(5);

    auto tl = PIXELS_TO_UV_COORDS(128, 512, 0, 0, 35, 35);
    auto tc = PIXELS_TO_UV_COORDS(128, 512, 51, 0, 5, 35);
    auto tr = PIXELS_TO_UV_COORDS(128, 512, 70, 0, 35, 35);
    auto cl = PIXELS_TO_UV_COORDS(128, 512, 0, 35, 35, 235);
    auto cc = PIXELS_TO_UV_COORDS(128, 512, 51, 35, 5, 235);
    auto cr = PIXELS_TO_UV_COORDS(128, 512, 70, 35, 35, 235);
    auto bl = PIXELS_TO_UV_COORDS(128, 512, 0, 270, 35, 40);
    auto bc = PIXELS_TO_UV_COORDS(128, 512, 51, 270, 5, 40);
    auto br = PIXELS_TO_UV_COORDS(128, 512, 70, 270, 35, 40);

    auto colour = IM_COL32(255, 255, 255, 255 * alpha);

    drawList->AddImage(g_texGeneralWindow.get(), min, { min.x + commonWidth, min.y + commonHeight }, GET_UV_COORDS(tl), colour);
    drawList->AddImage(g_texGeneralWindow.get(), { min.x + commonWidth, min.y }, { max.x - commonWidth, min.y + commonHeight }, GET_UV_COORDS(tc), colour);
    drawList->AddImage(g_texGeneralWindow.get(), { max.x - commonWidth, min.y }, { max.x, min.y + commonHeight }, GET_UV_COORDS(tr), colour);
    drawList->AddImage(g_texGeneralWindow.get(), { min.x, min.y + commonHeight }, { min.x + commonWidth, max.y - commonHeight }, GET_UV_COORDS(cl), colour);
    drawList->AddImage(g_texGeneralWindow.get(), { min.x + commonWidth, min.y + commonHeight }, { max.x - commonWidth, max.y - commonHeight }, GET_UV_COORDS(cc), colour);
    drawList->AddImage(g_texGeneralWindow.get(), { max.x - commonWidth, min.y + commonHeight }, { max.x, max.y - commonHeight }, GET_UV_COORDS(cr), colour);
    drawList->AddImage(g_texGeneralWindow.get(), { min.x, max.y - commonHeight }, { min.x + commonWidth, max.y + bottomHeight }, GET_UV_COORDS(bl), colour);
    drawList->AddImage(g_texGeneralWindow.get(), { min.x + commonWidth, max.y - commonHeight }, { max.x - commonWidth, max.y + bottomHeight }, GET_UV_COORDS(bc), colour);
    drawList->AddImage(g_texGeneralWindow.get(), { max.x - commonWidth, max.y - commonHeight }, { max.x, max.y + bottomHeight }, GET_UV_COORDS(br), colour);
}

void DrawPauseHeaderContainer(ImVec2 min, ImVec2 max, float alpha)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    auto commonWidth = Scale(35);

    auto left = PIXELS_TO_UV_COORDS(128, 512, 0, 314, 35, 60);
    auto centre = PIXELS_TO_UV_COORDS(128, 512, 51, 314, 5, 60);
    auto right = PIXELS_TO_UV_COORDS(128, 512, 70, 314, 35, 60);

    auto colour = IM_COL32(255, 255, 255, 255 * alpha);

    drawList->AddImage(g_texGeneralWindow.get(), min, { min.x + commonWidth, max.y }, GET_UV_COORDS(left), colour);
    drawList->AddImage(g_texGeneralWindow.get(), { min.x + commonWidth, min.y }, { max.x - commonWidth, max.y }, GET_UV_COORDS(centre), colour);
    drawList->AddImage(g_texGeneralWindow.get(), { max.x - commonWidth, min.y }, max, GET_UV_COORDS(right), colour);
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
    {
        DrawRubyAnnotatedText
        (
            font,
            fontSize,
            FLT_MAX,
            { textX, position.y },
            0.0f,
            text,
            [=](const char* str, ImVec2 pos)
            {
                DrawTextBasic(font, fontSize, pos, color, str);
            },
            [=](const char* str, float size, ImVec2 pos)
            {
                DrawTextBasic(font, size, pos, color, str);
            }
        );
    }

    if (textX + textSize.x < position.x)
    {
        DrawRubyAnnotatedText
        (
            font,
            fontSize,
            FLT_MAX,
            { textX + textSize.x + rectWidth, position.y },
            0.0f,
            text,
            [=](const char* str, ImVec2 pos)
            {
                DrawTextBasic(font, fontSize, pos, color, str);
            },
            [=](const char* str, float size, ImVec2 pos)
            {
                DrawTextBasic(font, size, pos, color, str);
            }
        );
    }

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

    // Original 4:3 has thicker text shadows.
    if (Config::AspectRatio == EAspectRatio::OriginalNarrow)
    {
        radius *= 1.5f;
    }

    SetOutline(radius);
    drawList->AddText(font, fontSize, { pos.x + offset, pos.y + offset }, shadowColour, text);
    ResetOutline();

    drawList->AddText(font, fontSize, pos, colour, text, nullptr);
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

std::pair<std::string, std::map<std::string, std::string>> RemoveRubyAnnotations(const char* input)
{
    std::string output;
    std::map<std::string, std::string> rubyMap;
    std::string currentMain, currentRuby;
    size_t idx = 0;

    while (input[idx] != '\0')
    {
        if (input[idx] == '[')
        {
            idx++;
            currentMain.clear();
            currentRuby.clear();

            while (input[idx] != ':' && input[idx] != ']' && input[idx] != '\0')
            {
                currentMain += input[idx++];
            }
            if (input[idx] == ':')
            {
                idx++;
                while (input[idx] != ']' && input[idx] != '\0')
                {
                    currentRuby += input[idx++];
                }
            }
            if (input[idx] == ']') 
            {
                idx++;
            }

            if (!currentMain.empty() && !currentRuby.empty())
            {
                rubyMap[currentMain] = currentRuby;
            }

            output += currentMain;
        }
        else
        {
            output += input[idx++];
        }
    }

    return { output, rubyMap };
}

std::string ReAddRubyAnnotations(const std::string_view& wrappedText, const std::map<std::string, std::string>& rubyMap)
{
    std::string annotatedText;
    size_t idx = 0;
    size_t length = wrappedText.length();

    while (idx < length) {
        bool matched = false;
        for (const auto& [mainText, rubyText] : rubyMap)
        {
            if (wrappedText.substr(idx, mainText.length()) == mainText) 
            {
                annotatedText += "[";
                annotatedText += mainText;
                annotatedText += ":";
                annotatedText += rubyText;
                annotatedText += "]";

                idx += mainText.length();
                matched = true;
                break;
            }
        }
        if (!matched)
        {
            annotatedText += wrappedText[idx++];
        }
    }

    return annotatedText;
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

Paragraph CalculateAnnotatedParagraph(const std::vector<std::string>& lines)
{
    Paragraph result;

    for (const auto& line : lines)
    {
        std::vector<TextSegment> segments;

        size_t pos = 0;
        size_t start = 0;

        while ((start = line.find('[', pos)) != std::string::npos)
        {
            size_t end = line.find(']', start);
            if (end == std::string::npos)
                break;

            size_t colon = line.find(':', start);
            if (colon != std::string::npos && colon < end)
            {
                if (start != pos)
                {
                    segments.push_back({ false, line.substr(pos, start - pos), "" });
                }

                segments.push_back({ true, line.substr(start + 1, colon - start - 1), line.substr(colon + 1, end - colon - 1) });

                result.annotated = true;
                pos = end + 1;
            }
            else
            {
                pos = start + 1;
            }
        }

        if (pos < line.size())
        {
            segments.push_back({ false, line.substr(pos), "" });
        }

        result.lines.push_back(segments);
    }

    return result;
}

std::vector<std::string> RemoveAnnotationFromParagraph(const std::vector<std::string>& lines)
{
    std::vector<std::string> result;
    const auto paragraph = CalculateAnnotatedParagraph(lines);

    for (auto& annotatedLine : paragraph.lines)
    {
        std::string annotationRemovedLine = "";

        for (const auto& segment : annotatedLine)
            annotationRemovedLine += segment.text;

        result.push_back(annotationRemovedLine);
    }

    return result;
}

std::string RemoveAnnotationFromParagraphLine(const std::vector<TextSegment>& annotatedLine)
{
    std::string result = "";

    for (auto& segment : annotatedLine)
        result += segment.text;

    return result;
}

ImVec2 MeasureCentredParagraph(const ImFont* font, float fontSize, float lineMargin, const std::vector<std::string>& lines)
{
    auto x = 0.0f;
    auto y = 0.0f;

    const auto paragraph = CalculateAnnotatedParagraph(lines);
    
    std::vector<std::string> annotationRemovedLines;

    for (const auto& line : paragraph.lines)
        annotationRemovedLines.emplace_back(RemoveAnnotationFromParagraphLine(line));

    for (size_t i = 0; i < annotationRemovedLines.size(); i++)
    {
        auto textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, annotationRemovedLines[i].c_str());

        x = std::max(x, textSize.x);
        y += textSize.y + Scale(lineMargin);

        if (paragraph.annotated && i != (annotationRemovedLines.size() - 1))
            y += fontSize * ANNOTATION_FONT_SIZE_MODIFIER;
    }

    return { x, y };
}

ImVec2 MeasureCentredParagraph(const ImFont* font, float fontSize, float maxWidth, float lineMargin, const char* text)
{
    const auto input = RemoveRubyAnnotations(text);
    auto lines = Split(input.first.c_str(), font, fontSize, maxWidth);

    for (auto& line : lines)
        line = ReAddRubyAnnotations(line, input.second);

    return MeasureCentredParagraph(font, fontSize, lineMargin, lines);
}

void DrawRubyAnnotatedText(const ImFont* font, float fontSize, float maxWidth, const ImVec2& pos, float lineMargin, const char* text, std::function<void(const char*, ImVec2)> drawMethod, std::function<void(const char*, float, ImVec2)> annotationDrawMethod, bool isCentred, bool leadingSpace)
{
    auto annotationFontSize = fontSize * ANNOTATION_FONT_SIZE_MODIFIER;

    const auto input = RemoveRubyAnnotations(text);
    auto lines = Split(input.first.c_str(), font, fontSize, maxWidth);

    for (auto& line : lines)
    {
        line = ReAddRubyAnnotations(line, input.second);
        if (!line.empty() && line.substr(0, 3) != "「" && leadingSpace)
        {
            line.insert(0, " ");
        }
    }

    auto paragraphSize = MeasureCentredParagraph(font, fontSize, lineMargin, lines);
    auto offsetY = 0.0f;

    const auto paragraph = CalculateAnnotatedParagraph(lines);

    for (const auto& annotatedLine : paragraph.lines)
    {
        const auto annotationRemovedLine = RemoveAnnotationFromParagraphLine(annotatedLine);

        auto textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, annotationRemovedLine.c_str());
        auto annotationSize = font->CalcTextSizeA(annotationFontSize, FLT_MAX, 0, "");
        auto textX = pos.x;
        auto textY = pos.y + offsetY;

        if (isCentred)
        {
            textX = annotationRemovedLine.starts_with("- ")
                ? pos.x - paragraphSize.x / 2
                : pos.x - textSize.x / 2;

            textY = pos.y - paragraphSize.y / 2 + offsetY;
        }

        for (const auto& segment : annotatedLine)
        {
            textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, segment.text.c_str());

            if (segment.annotated)
            {
                annotationSize = font->CalcTextSizeA(annotationFontSize, FLT_MAX, 0, segment.annotation.c_str());
                float annotationX = textX + (textSize.x - annotationSize.x) / 2.0f;

                annotationDrawMethod(segment.annotation.c_str(), annotationFontSize, { annotationX, textY - annotationFontSize });
            }

            drawMethod(segment.text.c_str(), { textX, textY });
            
            textX += textSize.x;
        }

        offsetY += textSize.y + Scale(lineMargin);

        if (paragraph.annotated)
            offsetY += annotationSize.y;
    }
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

void DrawVersionString(const ImFont* font, const ImU32 col)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    auto& res = ImGui::GetIO().DisplaySize;
    auto fontSize = Scale(12);
    auto textMargin = Scale(2);
    auto textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0, g_versionString);

    drawList->AddText(font, fontSize, { res.x - textSize.x - textMargin, res.y - textSize.y - textMargin }, col, g_versionString);
}

void DrawSelectionContainer(ImVec2 min, ImVec2 max, bool fadeTop)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    static auto breatheStart = ImGui::GetTime();
    auto alpha = BREATHE_MOTION(1.0f, 0.55f, breatheStart, 0.92f);
    auto colour = IM_COL32(255, 255, 255, 255 * alpha);
    auto commonWidth = Scale(11);
    auto commonHeight = Scale(24);

    if (fadeTop)
    {
        auto left = PIXELS_TO_UV_COORDS(64, 64, 0, 0, 11, 50);
        auto centre = PIXELS_TO_UV_COORDS(64, 64, 11, 0, 8, 50);
        auto right = PIXELS_TO_UV_COORDS(64, 64, 19, 0, 11, 50);

        drawList->AddImage(g_texSelect.get(), min, { min.x + commonWidth, max.y }, GET_UV_COORDS(left), colour);
        drawList->AddImage(g_texSelect.get(), { min.x + commonWidth, min.y }, { max.x - commonWidth, max.y }, GET_UV_COORDS(centre), colour);
        drawList->AddImage(g_texSelect.get(), { max.x - commonWidth, min.y }, max, GET_UV_COORDS(right), colour);

        return;
    }

    auto tl = PIXELS_TO_UV_COORDS(64, 64, 34, 0, 11, 24);
    auto tc = PIXELS_TO_UV_COORDS(64, 64, 45, 0, 8, 24);
    auto tr = PIXELS_TO_UV_COORDS(64, 64, 53, 0, 11, 24);
    auto cl = PIXELS_TO_UV_COORDS(64, 64, 34, 24, 11, 2);
    auto cc = PIXELS_TO_UV_COORDS(64, 64, 45, 24, 8, 2);
    auto cr = PIXELS_TO_UV_COORDS(64, 64, 53, 24, 11, 2);
    auto bl = PIXELS_TO_UV_COORDS(64, 64, 34, 26, 11, 24);
    auto bc = PIXELS_TO_UV_COORDS(64, 64, 45, 26, 8, 24);
    auto br = PIXELS_TO_UV_COORDS(64, 64, 53, 26, 11, 24);

    drawList->AddImage(g_texSelect.get(), min, { min.x + commonWidth, min.y + commonHeight }, GET_UV_COORDS(tl), colour);
    drawList->AddImage(g_texSelect.get(), { min.x + commonWidth, min.y }, { max.x - commonWidth, min.y + commonHeight }, GET_UV_COORDS(tc), colour);
    drawList->AddImage(g_texSelect.get(), { max.x - commonWidth, min.y }, { max.x, min.y + commonHeight }, GET_UV_COORDS(tr), colour);
    drawList->AddImage(g_texSelect.get(), { min.x, min.y + commonHeight }, { min.x + commonWidth, max.y - commonHeight }, GET_UV_COORDS(cl), colour);
    drawList->AddImage(g_texSelect.get(), { min.x + commonWidth, min.y + commonHeight }, { max.x - commonWidth, max.y - commonHeight }, GET_UV_COORDS(cc), colour);
    drawList->AddImage(g_texSelect.get(), { max.x - commonWidth, min.y + commonHeight }, { max.x, max.y - commonHeight }, GET_UV_COORDS(cr), colour);
    drawList->AddImage(g_texSelect.get(), { min.x, max.y - commonHeight }, { min.x + commonWidth, max.y }, GET_UV_COORDS(bl), colour);
    drawList->AddImage(g_texSelect.get(), { min.x + commonWidth, max.y - commonHeight }, { max.x - commonWidth, max.y }, GET_UV_COORDS(bc), colour);
    drawList->AddImage(g_texSelect.get(), { max.x - commonWidth, max.y - commonHeight }, { max.x, max.y }, GET_UV_COORDS(br), colour);
}

void DrawToggleLight(ImVec2 pos, bool isEnabled, float alpha)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    auto lightSize = Scale(14);
    auto lightCol = IM_COL32(255, 255, 255, 255 * alpha);

    ImVec2 min = { pos.x, pos.y };
    ImVec2 max = { min.x + lightSize, min.y + lightSize };

    if (isEnabled)
    {
        auto lightGlowSize = Scale(24);
        auto lightGlowUVs = PIXELS_TO_UV_COORDS(64, 64, 31, 31, 32, 32);

        ImVec2 lightGlowMin = { min.x - (lightGlowSize / 2) + Scale(2), min.y - lightGlowSize / 2 };
        ImVec2 lightGlowMax = { min.x + lightGlowSize, min.y + lightGlowSize };

        SetAdditive(true);
        drawList->AddImage(g_texLight.get(), lightGlowMin, lightGlowMax, GET_UV_COORDS(lightGlowUVs), IM_COL32(255, 255, 0, 127 * alpha));
        SetAdditive(false);

        auto lightOnUVs = PIXELS_TO_UV_COORDS(64, 64, 14, 0, 14, 14);

        drawList->AddImage(g_texLight.get(), min, max, GET_UV_COORDS(lightOnUVs), lightCol);
    }
    else
    {
        auto lightOffUVs = PIXELS_TO_UV_COORDS(64, 64, 0, 0, 14, 14);

        drawList->AddImage(g_texLight.get(), min, max, GET_UV_COORDS(lightOffUVs), lightCol);
    }
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
