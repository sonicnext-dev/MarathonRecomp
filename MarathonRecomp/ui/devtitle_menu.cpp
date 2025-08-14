#include "devtitle_menu.h"
#include "imgui_utils.h"
#include <gpu/imgui/imgui_snapshot.h>
#include <app.h>
#include <hid/hid.h>
#include <exports.h>
#include <apu/embedded_player.h>
#include <sdl_listener.h>

static ImFont* g_mrodinFont;
static ImFont* g_mnewRodinFont;
const float ScrollAmount = 0.25f;
bool DevTitleMenu::IsVisible = true;

const float DevTitleDelayDefault = 1.0;
static float DevTitleDelay = DevTitleDelayDefault;


enum CenterFlag {
    X = 1,
    Y = 2
};

struct DevMessage
{
    be<uint32_t> id;
    be<uint32_t> id2;
    uint8_t id3;
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

    const uint32_t TOP = IM_COL32(0, 103, 255, 255);
    const uint32_t BOTTOM = IM_COL32(0, 40, 100, 255);

    drawList->AddRectFilledMultiColor({ 0.0, 0.0 }, res, TOP, TOP, BOTTOM, BOTTOM);
}

static void DrawTextDev(const char* text,float posX, float posY,float vfontSize= 24, uint32_t alpha = 255, uint32_t center = CenterFlag::X)
{
    auto drawList = ImGui::GetBackgroundDrawList();
    auto fontSize = Scale(vfontSize);
    auto textSize = g_mnewRodinFont->CalcTextSizeA(fontSize, FLT_MAX, 0, text);
    auto cornerRadius = 23;

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

//05.08.2025 TODO Kanju Display
static std::string ConvertShiftJISToUTF8(const std::string& sjisStr)
{

    int wlen = MultiByteToWideChar(932, 0, sjisStr.c_str(), -1, nullptr, 0);
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(932, 0, sjisStr.c_str(), -1, &wstr[0], wlen);
    int ulen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8Str(ulen, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], ulen, nullptr, nullptr);
    return utf8Str;
}

static void IMAddRectangle(ImDrawList* list,const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_left,ImU32 col_bot_right)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    const ImVec2 uv = list->_Data->TexUvWhitePixel;
    list->PrimReserve(6, 4);
    list->PrimWriteIdx((ImDrawIdx)(list->_VtxCurrentIdx));  list->PrimWriteIdx((ImDrawIdx)(list->_VtxCurrentIdx + 1));  list->PrimWriteIdx((ImDrawIdx)(list->_VtxCurrentIdx + 2));
    list->PrimWriteIdx((ImDrawIdx)(list->_VtxCurrentIdx));  list->PrimWriteIdx((ImDrawIdx)(list->_VtxCurrentIdx + 2));  list->PrimWriteIdx((ImDrawIdx)(list->_VtxCurrentIdx + 3));
    list->PrimWriteVtx(p1, uv, col_upr_left);
    list->PrimWriteVtx(p2, uv, col_upr_right);
    list->PrimWriteVtx(p3, uv, col_bot_right);
    list->PrimWriteVtx(p4, uv, col_bot_left);
}

static bool Sonic06Button(ImVec2 pos, const char* label, bool& hovered, const ImVec2& size_arg = {},
    bool selected = false, uint32_t center_flags = 0)
{
    auto* draw_list = ImGui::GetBackgroundDrawList();
    auto& io = ImGui::GetIO();
    const float font_size = size_arg.y / 2.5;
    const float skew_amount = 1.0f;

    const auto label_size = g_mnewRodinFont->CalcTextSizeA(font_size, FLT_MAX, 0, label);
    const auto size = ImVec2(size_arg.x ? size_arg.x : label_size.x, size_arg.y ? size_arg.y : label_size.y);
    const float skew = size.y * skew_amount;
    
    const ImRect bb(pos, { pos.x + size.x + skew, pos.y + size.y });
    hovered = bb.Contains(GetMousePos());
    bool pressed = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    // Colors
    const ImU32 col_base IM_COL32(0, 6, 142, 255), col_hover IM_COL32(0, 26, 162, 255);

    const ImU32 bg[4] =
    {
     hovered ? col_hover : selected ? col_base : 0,

     hovered ? col_hover : selected ? col_base : 0,

     hovered ? col_hover : selected ? col_base : 0,

     hovered ? col_hover : selected ? col_base : 0,
    };


    const ImVec2 pts[] = {
        pos,                                    // top-left
        {pos.x + skew, pos.y + size.y},         // bottom-left
        {pos.x + size.x , pos.y + size.y},      // bottom-right
        {pos.x + size.x, pos.y}                 // top-right
    };
    IMAddRectangle(draw_list, pts[0], pts[1], pts[2], pts[3], bg[0], bg[1], bg[2], bg[3]);

    // Text
    ImVec2 text_pos = { pos.x + (2.0f * skew), pos.y };

    if (center_flags & CenterFlag::X)
        text_pos.x = bb.GetCenter().x - label_size.x * 0.5f;
    if (center_flags & CenterFlag::Y)
        text_pos.y = bb.GetCenter().y - label_size.y * 0.5f;

    draw_list->AddText(g_mnewRodinFont, font_size, { text_pos.x + 1,text_pos.y + 1 }, IM_COL32(0, 0, 0, 120), label);
    draw_list->AddText(g_mnewRodinFont, font_size, text_pos, IM_COL32(255, 255, 255, 255), label);

    return pressed;
}

static void DrawStageMapSelector()
{
    auto& io = ImGui::GetIO();
    auto& res = io.DisplaySize;

    auto* pMode = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::StageSelectMode>();
    if (!pMode)
        return;

    DrawBackgroundDev();
    DrawHUD({ 0, 0 }, res, g_mnewRodinFont, pMode->m_StageMapName.get());

    const float WINDOW_WIDTH = 0.8f * res.x;
    const float BUTTON_HEIGHT = 0.05f * res.y;
    const float BUTTON_SPACING = 5.0f;
    const int VISIBLE_ITEMS = 9;
    const float ITEMS_SCALE_HEIGHT = 0.05f * Video::s_viewportHeight;
    const float WINDOW_HEIGHT = (BUTTON_HEIGHT + BUTTON_SPACING) * VISIBLE_ITEMS + ITEMS_SCALE_HEIGHT;

    // Calculate base position for the selector
    const ImVec2 base_pos = {
        (res.x - WINDOW_WIDTH) * 0.15f,
        (res.y - WINDOW_HEIGHT) * 0.45f
    };

    auto& items = pMode->m_CurrentStageMap->m_vpStageMap;
    const int itemCount = static_cast<int>(items.size());
    int currentIdx = pMode->m_CurrentStageMapIndex.get();

    // Input handling (Mouse)
    auto pMsgRec = reinterpret_cast<uintptr_t>(static_cast<Sonicteam::SoX::MessageReceiver*>(pMode));

    if (abs(io.MouseWheel) > ScrollAmount) {
        guest_stack_var<DevMessage> vMsgRec(0x10001, io.MouseWheel > 0 ? 0 : 0xB4, 130);
        GuestToHostFunction<void>(sub_824A8FF0, pMsgRec, vMsgRec.get());
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        guest_stack_var<DevMessage> vMsgRec(0x10002, 0, 0);
        GuestToHostFunction<void>(sub_824A8FF0, pMsgRec, vMsgRec.get());
    }

    // Calculate visible range
    const int start_idx = std::max(0, currentIdx - VISIBLE_ITEMS / 2);
    const int end_idx = std::min(itemCount, start_idx + VISIBLE_ITEMS);

    // Draw visible items
    for (int i = start_idx; i < end_idx; ++i) {
        const auto& item = items[i];
        if (!item) continue;

        const bool isSelected = (i == currentIdx);
        const int visible_index = i - start_idx; 
        bool hovered = false;

        // Calculate button position
        const ImVec2 button_pos = {
            base_pos.x + 10,
            base_pos.y + 40 + visible_index * (BUTTON_HEIGHT + BUTTON_SPACING)
        };

        if (Sonic06Button(button_pos, item->m_Name.c_str(),hovered,
            ImVec2(WINDOW_WIDTH - 20, BUTTON_HEIGHT), isSelected,CenterFlag::Y))
        {
            currentIdx = i;
            pMode->m_CurrentStageMapIndex = i;
            guest_stack_var<DevMessage> vMsgRec(0x10002, 0x5A, 1);
            GuestToHostFunction<void>(sub_824A8FF0, pMsgRec, vMsgRec.get());
        }

        // Tooltip with stage information
        if (hovered && item->m_vpStageMap.size() > 0) {
            ImDrawList* draw_list = ImGui::GetForegroundDrawList();
            const ImVec2 mouse_pos = GetMousePos();
            const float font_size = ImGui::GetFontSize();
            const float line_spacing = 2.0f;


            float max_name_width = 0.0f;
            float max_text_width = 0.0f;
            float total_height = 0.0f;
            std::vector<std::pair<std::string, std::string>> entries;

            for (int j = 0; j < item->m_vpStageMap.size(); ++j) {
                const auto& stage = item->m_vpStageMap[j];
                if (!stage.ptr.get()) continue;

                std::string nameUtf8 = ConvertShiftJISToUTF8(stage->m_Name.c_str());
                std::string textUtf8 = ConvertShiftJISToUTF8(stage->m_Text.c_str());
                entries.emplace_back(nameUtf8, textUtf8);

                // Measure each part separately using the actual rendering font
                const ImVec2 name_size = g_mnewRodinFont->CalcTextSizeA(font_size, FLT_MAX, 0.0f, nameUtf8.c_str());
                const ImVec2 text_size = g_mnewRodinFont->CalcTextSizeA(font_size, FLT_MAX, 0.0f, textUtf8.c_str());

                max_name_width = ImMax(max_name_width, name_size.x);
                max_text_width = ImMax(max_text_width, text_size.x);
                total_height += ImMax(name_size.y, text_size.y) + line_spacing;
            }

            const float text_padding = 10.0f;
            const float separator_width = 5.0f;
            const float tooltip_padding = 15.0f;
            const float total_width = max_name_width + separator_width + max_text_width + tooltip_padding * 2;
            total_height += tooltip_padding * 2 - line_spacing; 

            // Position tooltip (ensure it stays on screen)
            ImVec2 tooltip_pos = ImVec2(
                ImClamp(mouse_pos.x - total_width * 0.5f, 10.0f, io.DisplaySize.x - total_width - 10.0f),
                ImClamp(mouse_pos.y - total_height - 20.0f, 10.0f, io.DisplaySize.y - total_height - 10.0f)
            );
            if (tooltip_pos.y <= 10)
            {
                tooltip_pos.x += io.DisplaySize.x * 0.040;
            }

            // Draw background
            const ImVec2 tooltip_min = tooltip_pos;
            const ImVec2 tooltip_max = ImVec2(tooltip_pos.x + total_width, tooltip_pos.y + total_height);
            draw_list->AddRectFilled(tooltip_min, tooltip_max, IM_COL32(0, 0, 0, 180), 5.0f);
            draw_list->AddRect(tooltip_min, tooltip_max, IM_COL32(50, 150, 255, 200), 5.0f, 0, 1.5f);

            // Draw text entries
            float current_y = tooltip_pos.y + tooltip_padding;
            for (const auto& [name, text] : entries) {
                // Calculate this line's height
                const float name_height = g_mnewRodinFont->CalcTextSizeA(font_size, FLT_MAX, 0.0f, name.c_str()).y;
                const float text_height = g_mnewRodinFont->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text.c_str()).y;
                const float line_height = ImMax(name_height, text_height);

                // Draw name (left-aligned)
                draw_list->AddText(
                    g_mnewRodinFont,
                    font_size,
                    ImVec2(tooltip_pos.x + tooltip_padding, current_y),
                    IM_COL32(255, 255, 255, 255),
                    name.c_str()
                );

                // Draw text (right-aligned)
                const float text_width = g_mnewRodinFont->CalcTextSizeA(font_size, FLT_MAX, 0.0f, text.c_str()).x;
                draw_list->AddText(
                    g_mnewRodinFont,
                    font_size,
                    ImVec2(tooltip_pos.x + tooltip_padding + max_name_width + separator_width, current_y),
                    IM_COL32(200, 200, 255, 255),
                    text.c_str()
                );

                current_y += line_height + line_spacing;
            }
        }
     
     
    }
    
    // Draw mouse position indicator
    const float dot_radius = 7.5f;
    const ImU32 dot_color = IM_COL32(0, 150, 255, 220);
    const ImU32 outline_color = IM_COL32(255, 255, 255, 180);
    ImVec2 mouse_pos = GetMousePos();

    ImGui::GetBackgroundDrawList()->AddCircleFilled(
        mouse_pos, dot_radius, dot_color, 12
    );
    ImGui::GetBackgroundDrawList()->AddCircle(
        mouse_pos, dot_radius, outline_color, 12, 1.5f
    );
}

static void DrawDevTitle() {

    auto& io = ImGui::GetIO();
    auto* pMode = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::DevTitleMode>();
    if (!pMode)
        return;

    auto pDevTitle = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::DevTitleMode>();
    auto pIndex = pDevTitle->m_Selected.get();
    auto pDevTitleMenuSTable = (DevTitleMenuSTable*)g_memory.Translate(0x82B10A40);
    auto pName = fmt::format(">>[{}]<<", pDevTitleMenuSTable->m_state[pIndex].m_name.get());

  
    DrawTextDev(pName.c_str(), 0.5, 0.95, 24, DevTitleDelay * 255, CenterFlag::X | CenterFlag::Y);
    auto pMsgRec = (uintptr_t)static_cast<Sonicteam::SoX::MessageReceiver*>(pMode);


    if ((abs(io.MouseWheel) > ScrollAmount))
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

static void DevTitleAudioCommon(DevMessage* message)
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

//Add audio to MessageReciever(StageSelectMode)
PPC_FUNC_IMPL(__imp__sub_824A8FF0);
PPC_FUNC(sub_824A8FF0) 
{
    auto pMessage = (DevMessage*)(base + ctx.r4.u32);
    DevTitleAudioCommon(pMessage);
    __imp__sub_824A8FF0(ctx, base);
}

//Add audio to MessageReciever(DevTitleMode)
PPC_FUNC_IMPL(__imp__sub_824A0E38);
PPC_FUNC(sub_824A0E38)
{
    auto pMessage = (DevMessage*)(base + ctx.r4.u32);
    auto pTitle = (StateDevTitle*)(base + 0x82D35E6C);
    if (pTitle->m_PictureTitle->m_State3 >= 4)
        return;

    auto pTitleMode = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::DevTitleMode>();

    if (pMessage->id == 0x10002)
    {
        if (pMessage->id2 == 0x5A || pMessage->id3)
        {
            auto pPocture = pTitle->m_PictureTitle;
            pTitle->m_PictureTitle->m_State3 = 4;
            DevTitleDelay = DevTitleDelayDefault;
        }
        else
        {
            return;
        }
    }

    DevTitleAudioCommon(pMessage);
    __imp__sub_824A0E38(ctx, base);
}

PPC_FUNC_IMPL(__imp__sub_82161AB8);
PPC_FUNC(sub_82161AB8)
{
    if (!Config::DevTitle)
    {
        __imp__sub_82161AB8(ctx, base);
        return;
    }

    auto pDoc = (Sonicteam::DocMarathonImp*)(base + ctx.r3.u32);
    GuestToHostFunction<void>(sub_825E9E28, ctx.r3.u32, ctx.r4.u32);

    //Initialize MainMode
    if (!*(uint32_t*)(base + 0x82D35E48))
        *(be<uint32_t>*)(base + 0x82D35E48) = GuestToHostFunction<uint32_t>(sub_82161150);

    //Initialize DevMode
    if (!*(uint32_t*)(base + 0x82D35E78))
        *(be<uint32_t>*)(base + 0x82D35E78) = GuestToHostFunction<uint32_t>(sub_82161860);

    auto pDevMode = (*(be<uint32_t>*)(base + 0x82D35E78)).get();

    //nah vft call, sub_825E8D78 should do it
    //DocSetMode
    GuestToHostFunction<void>(sub_825E8D78, ctx.r3.u32, pDevMode);

}

//tateDevTitle, vft + 8
PPC_FUNC_IMPL(__imp__sub_82162298);
PPC_FUNC(sub_82162298)
{
    auto pDoc = (Sonicteam::DocMarathonState*)(base + ctx.r4.u32);
    auto pTitle = (StateDevTitle*)(base + ctx.r3.u32);

    if (ctx.r3.u32 && pTitle->m_PictureTitle && pTitle->m_PictureTitle->m_State3 > 4)
    {
        DevTitleDelay = std::clamp(DevTitleDelay - 0.016666668f, 0.0f, DevTitleDelay);
    }
    else
    {
        DevTitleDelay = DevTitleDelayDefault;
    }

    if (!GuestToHostFunction<uint32_t>(sub_82581C68, ctx.r4.u32)) 
    {
        //Wait for transistion
        printf("pTitle->m_PictureTitle->m_State %d\n", pTitle->m_PictureTitle->m_State3.get());
        GuestToHostFunction<uint32_t>(sub_824A0870, pTitle->m_PictureTitle.ptr.get(), 0.016666668);
       
        //Force to wait transistion
        if (pTitle->m_PictureTitle->m_State3 != 6) 
        {
            return;
        }   
    }
    __imp__sub_82162298(ctx, base);
}

void DevTitleMenu::Init() 
{
    if (!Config::DevTitle)
        return;

    g_mrodinFont = ImFontAtlasSnapshot::GetFont("FOT-RodinPro-DB.otf");
    g_mnewRodinFont = ImFontAtlasSnapshot::GetFont("FOT-NewRodinPro-UB.otf");
    
}
void DevTitleMenu::Draw()
{

    if (!Config::DevTitle)
        return;

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
        //DevTitleMode
        case 0x8202B1E0:
        {
            DevTitleMenu::IsVisible = true;
            DrawDevTitle();
        };
        break;
        //StageSelect/StoreSelect
        case 0x820336A4:
        {
            DevTitleMenu::IsVisible = true;
            DrawStageMapSelector();
        }
        break;
    defaut:
        if (EmbeddedPlayer::s_isActive && DevTitleMenu::IsVisible)
            DevTitleMenu::IsVisible = false;
            EmbeddedPlayer::Shutdown();
    }
}
