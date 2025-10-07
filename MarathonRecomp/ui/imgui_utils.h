#pragma once

#include <gpu/imgui/imgui_common.h>
#include <gpu/video.h>

#define PIXELS_TO_UV_COORDS(textureWidth, textureHeight, x, y, width, height) \
    std::make_tuple(ImVec2((float)x / (float)textureWidth, (float)y / (float)textureHeight), \
                    ImVec2(((float)x + (float)width) / (float)textureWidth, ((float)y + (float)height) / (float)textureHeight))

#define GET_UV_COORDS(tuple) std::get<0>(tuple), std::get<1>(tuple)

#define CENTRE_TEXT_HORZ(min, max, textSize) min.x + ((max.x - min.x) - textSize.x) / 2
#define CENTRE_TEXT_VERT(min, max, textSize) min.y + ((max.y - min.y) - textSize.y) / 2

#define BREATHE_MOTION(start, end, time, rate) Lerp(start, end, (sin((ImGui::GetTime() - time) * (2.0f * M_PI / rate)) + 1.0f) / 2.0f)

#define IM_COL32_WHITE_TRANS IM_COL32(255, 255, 255, 0)

extern ImFont* g_pFntRodin;
extern ImFont* g_pFntNewRodin;

extern std::unique_ptr<GuestTexture> g_upTexWindow;
extern std::unique_ptr<GuestTexture> g_upTexLight;
extern std::unique_ptr<GuestTexture> g_upTexSelect;
extern std::unique_ptr<GuestTexture> g_upTexSelectArrow;
extern std::unique_ptr<GuestTexture> g_upTexMainMenu1;

void InitImGuiUtils();

void SetGradient(const ImVec2& min, const ImVec2& max, ImU32 top, ImU32 bottom);
void SetHorizontalGradient(const ImVec2& min, const ImVec2& max, ImU32 left, ImU32 right);
void SetGradient(const ImVec2& min, const ImVec2& max, ImU32 topLeft, ImU32 topRight, ImU32 bottomRight, ImU32 bottomLeft);
void ResetGradient();
void SetShaderModifier(uint32_t shaderModifier);
void SetOrigin(ImVec2 origin);
void SetScale(ImVec2 scale);
void SetTextSkew(float yCenter, float skewScale);
void ResetTextSkew();
void SetHorizontalMarqueeFade(ImVec2 min, ImVec2 max, float fadeScaleLeft, float fadeScaleRight);
void SetHorizontalMarqueeFade(ImVec2 min, ImVec2 max, float fadeScale);
void SetVerticalMarqueeFade(ImVec2 min, ImVec2 max, float fadeScaleTop, float fadeScaleBottom);
void SetVerticalMarqueeFade(ImVec2 min, ImVec2 max, float fadeScale);
void ResetMarqueeFade();
void SetOutline(float outline);
void ResetOutline();
void SetProceduralOrigin(ImVec2 proceduralOrigin);
void ResetProceduralOrigin();
void SetAdditive(bool enabled);
void ResetAdditive();
void AddImageFlipped(ImTextureID texture, const ImVec2& min, const ImVec2& max, const ImVec2& uvMin = { 0, 0 }, const ImVec2& uvMax = { 0, 0 }, ImU32 col = IM_COL32_WHITE, bool flipHorz = false, bool flipVert = false);
float Scale(float size, bool useGameplayScale = false);
double ComputeLoopMotion(double time, double offset, double total);
double ComputeLinearMotion(double time, double offset, double total, bool reverse = false);
double ComputeMotion(double time, double offset, double total, bool reverse = false);
void DrawArrows(ImVec2 min, ImVec2 max);
double ImValueDebug(double& value, double increment = 1.0);
void DrawContainerBox(ImVec2 min, ImVec2 max, float alpha = 1);
void DrawTextBasic(const ImFont* font, float fontSize, const ImVec2& pos, ImU32 colour, const char* text);
void DrawTextWithMarquee(const ImFont* font, float fontSize, const ImVec2& position, const ImVec2& min, const ImVec2& max, ImU32 color, const char* text, double time, double delay, double speed);
void DrawTextWithMarqueeShadow(const ImFont* font, float fontSize, const ImVec2& pos, const ImVec2& min, const ImVec2& max, ImU32 colour, const char* text, double time, double delay, double speed, float offset = 2.0f, float radius = 1.0f, ImU32 shadowColour = IM_COL32(0, 0, 0, 255));
void DrawTextWithOutline(const ImFont* font, float fontSize, const ImVec2& pos, ImU32 color, const char* text, float outlineSize, ImU32 outlineColor, uint32_t shaderModifier = IMGUI_SHADER_MODIFIER_NONE);
void DrawTextWithShadow(const ImFont* font, float fontSize, const ImVec2& pos, ImU32 colour, const char* text, float offset = 2.0f, float radius = 1.0f, ImU32 shadowColour = IM_COL32(0, 0, 0, 255));
void DrawTextParagraph(const ImFont* font, float fontSize, float maxWidth, const ImVec2& pos, float lineMargin, const char* text, std::function<void(const char*, ImVec2)> drawMethod, bool isCentred = false);
float CalcWidestTextSize(const ImFont* font, float fontSize, std::span<std::string> strs);
std::string Truncate(const std::string& input, size_t maxLength, bool useEllipsis = true, bool usePrefixEllipsis = false);
std::vector<std::string> Split(const char* strStart, const ImFont* font, float fontSize, float maxWidth);
ImVec2 MeasureCentredParagraph(const ImFont* font, float fontSize, float lineMargin, const std::vector<std::string>& lines);
ImVec2 MeasureCentredParagraph(const ImFont* font, float fontSize, float maxWidth, float lineMargin, const char* text);
float Lerp(float a, float b, float t);
float Cubic(float a, float b, float t);
float Hermite(float a, float b, float t);
ImVec2 Lerp(const ImVec2& a, const ImVec2& b, float t);
ImU32 ColourLerp(ImU32 c0, ImU32 c1, float t);
void DrawVersionString(const ImFont* font, const ImU32 col = IM_COL32(255, 255, 255, 70));
void DrawToggleLight(ImVec2 pos, bool isEnabled, float alpha = 1.0f);
const char* CalcWordWrapPositionA(const ImFont* font, float scale, const char* text, const char* text_end, float wrap_width);
