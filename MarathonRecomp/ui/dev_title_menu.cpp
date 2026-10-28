#include "dev_title_menu.h"
#include "imgui_utils.h"
#include <gpu/imgui/imgui_snapshot.h>
#include <app.h>
#include <exports.h>
#include <apu/embedded_player.h>

static ImFont* g_mrodinFont;
static ImFont* g_mnewRodinFont;
const float ScrollAmount = 0.25f;
bool DevTitleMenu::IsVisible = true;

enum CenterFlag
{
    X = 1,
    Y = 2
};

struct DevTitleMenuSTable
{
    struct
    {
        xpointer<const char> m_name;
        xpointer<Sonicteam::DocState> m_mode;
    } m_state[5];
};

static ImVec2 GetMousePos()
{
    return ImGui::GetIO().MousePos;
}

static void DrawBackgroundDev()
{
    auto& res = ImGui::GetIO().DisplaySize;
    auto drawList = ImGui::GetBackgroundDrawList();

    auto color = IM_COL32(0, 0, 160, 255);
    drawList->AddRectFilled({ 0.0, 0.0 }, res, color);
}

static void DrawTextDev(const char* text, float posX, float posY, float vfontSize= 24, uint32_t alpha = 255, uint32_t center = X)
{
    auto fontSize = Scale(vfontSize);
    auto textSize = g_mnewRodinFont->CalcTextSizeA(fontSize, FLT_MAX, 0, text);

    float X = Video::s_viewportWidth * posX;
    float Y = Video::s_viewportHeight * posY;

    if (center & CenterFlag::X)
        X -= textSize.x * 0.5f;

    if (center & CenterFlag::Y)
        Y -= textSize.y * 0.5f;

    DrawTextWithOutline
    (
        g_mnewRodinFont,
        fontSize,
        { X,Y },
        IM_COL32(255, 255, 255, alpha),
        text,
        4,
        IM_COL32(0, 0, 0, alpha)
    );

    ResetTextSkew();
}

static bool Sonic06Button(ImVec2 pos, const char* label, bool& hovered, const ImVec2& sizeArg = {},
    bool selected = false, uint32_t centerFlags = 0, bool group = false)
{
    auto* drawList = ImGui::GetBackgroundDrawList();
    const float fontSize = sizeArg.y / 2.5f;
    const float gutter = fontSize * 1.5f;

    const auto labelSize = g_mnewRodinFont->CalcTextSizeA(fontSize, FLT_MAX, 0, label);
    const auto size = ImVec2(sizeArg.x ? sizeArg.x : gutter + labelSize.x, sizeArg.y ? sizeArg.y : labelSize.y);

    const auto mouse_pos = GetMousePos();
    const ImRect bb(pos, { pos.x + size.x, pos.y + size.y });
    const ImRect clip(drawList->GetClipRectMin(), drawList->GetClipRectMax());
    hovered = bb.Contains(mouse_pos) && clip.Contains(mouse_pos);
    const bool pressed = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    // Background
    const ImU32 bg_col = selected ? IM_COL32(255, 255, 255, 60) : hovered ? IM_COL32(255, 255, 255, 25) : 0;
    drawList->AddRectFilled(bb.Min, bb.Max, bg_col);

    // Text, indented past the gutter so labels align with and without a chevron
    ImVec2 textPos = { pos.x + gutter, pos.y };

    if (centerFlags & X)
        textPos.x = bb.GetCenter().x - labelSize.x * 0.5f;
    if (centerFlags & Y)
        textPos.y = bb.GetCenter().y - labelSize.y * 0.5f;

    drawList->AddText(g_mnewRodinFont, fontSize, { textPos.x + 1,textPos.y + 1 }, IM_COL32(0, 0, 0, 120), label);
    drawList->AddText(g_mnewRodinFont, fontSize, textPos, IM_COL32(255, 255, 255, 255), label);

    if (group)
    {
        // Chevron centred in the gutter to mark items that open a sub-list
        const float h = fontSize * 0.25f;
        const float x = pos.x + gutter * 0.5f;
        const float y = textPos.y + labelSize.y * 0.5f;
        drawList->AddTriangleFilled({ x - h, y - h }, { x + h, y }, { x - h, y + h }, IM_COL32(255, 255, 255, 255));
    }

    return pressed;
}

static void HandleStageMapSelectorInput(Sonicteam::StageSelectMode* pMode, uintptr_t pMsgRec)
{
    auto& io = ImGui::GetIO();

    // Mouse wheel scrolling
    if (abs(io.MouseWheel) > ScrollAmount)
    {
        guest_stack_var<DevMessage> vMsgRec(0x10001, io.MouseWheel > 0 ? 0 : 0xB4, 130);
        GuestToHostFunction<void>(sub_824A8FF0, pMsgRec, vMsgRec.get());
    }

    // Right click handling
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        guest_stack_var<DevMessage> vMsgRec(0x10002, 0, 0);
        GuestToHostFunction<void>(sub_824A8FF0, pMsgRec, vMsgRec.get());
    }
}

static void DrawStageMapSelector()
{
    auto& io = ImGui::GetIO();
    auto& res = io.DisplaySize;
    auto& config = s_StageMapSelectorConfig;

    auto* pMode = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::StageSelectMode>();
    if (!pMode) return;

    DrawBackgroundDev();

    // Calculate layout
    const int visibleItems = config.visibleItems;
    const float buttonHeight = config.buttonHeightRatio * res.y;
    const float rowHeight = buttonHeight * config.leading;
    const ImVec2 basePos = {
        config.paddingRatio.x * res.x,
        config.paddingRatio.y * res.y
    };

    auto& items = pMode->m_CurrentStageMap->m_Children;
    const int itemCount = static_cast<int>(items.size());
    int currentIdx = pMode->m_StageMapIndex.get();

    // Handle input
    auto pMsgRec = reinterpret_cast<uintptr_t>(static_cast<Sonicteam::SoX::MessageReceiver*>(pMode));
    HandleStageMapSelectorInput(pMode, pMsgRec);

    // Ease the scroll offset towards the selection, snapping when the list changes
    const int maxStart = std::max(0, itemCount - visibleItems);
    const float target = static_cast<float>(std::clamp(currentIdx - visibleItems / 2, 0, maxStart));
    const void* pStageMap = pMode->m_CurrentStageMap.get();

    if (pStageMap != s_pLastStageMap)
    {
        s_StageMapScroll = target;
        s_pLastStageMap = pStageMap;
    }
    else
    {
        s_StageMapScroll += (target - s_StageMapScroll) * (1.0f - expf(-config.scrollSpeed * io.DeltaTime));
    }

    // Calculate visible range, including partially visible items
    const int startIdx = static_cast<int>(s_StageMapScroll);
    const int endIdx = std::min(itemCount, static_cast<int>(ceilf(s_StageMapScroll)) + visibleItems);

    // Clip items scrolling in and out of the list
    auto* drawList = ImGui::GetBackgroundDrawList();
    drawList->PushClipRect(basePos, { res.x, basePos.y + visibleItems * rowHeight });

    // Draw visible items
    for (int i = startIdx; i < endIdx; ++i)
    {
        const auto& item = items[i];
        if (!item) continue;

        const bool isSelected = i == currentIdx;
        bool hovered = false;

        // Calculate button position
        const ImVec2 buttonPos = {
            basePos.x,
            basePos.y + (i - s_StageMapScroll) * rowHeight
        };

        // Draw button
        if (Sonic06Button(buttonPos, item->m_Name.c_str(), hovered,
            ImVec2(config.windowWidthRatio * res.x - 20, buttonHeight),
            isSelected, Y, item->m_Children.size() > 1))
        {
            currentIdx = i;
            pMode->m_StageMapIndex = i;
            guest_stack_var<DevMessage> vMsgRec(0x10002, 0x5A, 1);
            GuestToHostFunction<void>(sub_824A8FF0, pMsgRec, vMsgRec.get());
        }
    }

    drawList->PopClipRect();
}

static void DrawDevTitle()
{
    auto& io = ImGui::GetIO();
    auto* pMode = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::DevTitleMode>();
    if (!pMode)
        return;

    auto pDevTitle = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::DevTitleMode>();
    auto pIndex = pDevTitle->m_SelectedIndex.get();
    auto pDevTitleMenuSTable = static_cast<DevTitleMenuSTable *>(g_memory.Translate(0x82B10A40));
    auto pName = pDevTitleMenuSTable->m_state[pIndex].m_name.get();

    DrawTextDev(pName, 0.5, 0.95, 24, DevTitleDelay * 255, X | Y);
    auto pMsgRec = (uintptr_t)static_cast<Sonicteam::SoX::MessageReceiver*>(pMode);

    if (abs(io.MouseWheel) > ScrollAmount)
    {
        guest_stack_var<DevMessage> vMsgRec(0x10001, io.MouseWheel >= 0 ? 0 : 0xB4, 130);
        GuestToHostFunction<void>(sub_824A0E38, static_cast<uint32_t>(pMsgRec), vMsgRec.get());
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        guest_stack_var<DevMessage> vMsgRec(0x10002, 0, 1);
        GuestToHostFunction<void>(sub_824A0E38, static_cast<uint32_t>(pMsgRec), vMsgRec.get());
    }
}

void DevTitleMenu::Init()
{
    g_mrodinFont = ImFontAtlasSnapshot::GetFont("FOT-RodinPro-DB.otf");
    g_mnewRodinFont = ImFontAtlasSnapshot::GetFont("FOT-NewRodinPro-UB.otf");
}

void DevTitleMenu::Draw()
{
    if (!App::s_pApp)
        return;

    if (!App::s_pApp->m_pDoc)
        return;

    if (!App::s_pApp->m_pDoc->m_pDocMode)
        return;

    if (!EmbeddedPlayer::s_isActive)
        EmbeddedPlayer::Init();

    auto vft = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::SoX::Engine::Task::Object>()->m_pVftable.ptr.get();
    switch (vft)
    {
    // DevTitleMode
    case 0x8202B1E0:
        IsVisible = true;
        DrawDevTitle();
        break;
    // StageSelect/StoreSelect
    case 0x820336A4:
        IsVisible = true;
        DrawStageMapSelector();
        break;
    default:
        IsVisible = false;
        break;
    }
}

void DevTitleMenu::AudioCommon(DevMessage* message)
{
    switch (message->id)
    {
    case 0x10001:
        Game_PlaySound("move");
        break;
    case 0x10002:
        if (message->id2 == 0x5A || message->id3)
            Game_PlaySound("main_deside");
        else
            Game_PlaySound("window_close");
        break;
    case 0:
        if (!message->id3)
            Game_PlaySound("window_close");
        break;
    }
}
