#include "dev_title_menu.h"
#include <ui/imgui_utils.h>
#include <app.h>

static void DrawInfo()
{
    const auto drawList = ImGui::GetBackgroundDrawList();
    const auto fontSize = Scale(22);

    drawList->AddText(g_pFntRodin, fontSize, { 0, Scale(4) }, IM_COL32_WHITE, "ソニック・ザ・ヘッジホッグ");
    drawList->AddText(g_pFntRodin, Scale(11), { Scale(350), Scale(34) }, IM_COL32_WHITE, "Mon Oct 23 05:01:13     2006");
}

static void DrawItem(const ImVec2& in_rPosition, const char* in_pText, bool in_isSelected = false)
{
    const auto drawList = ImGui::GetBackgroundDrawList();
    const auto fontSize = Scale(22);

    if (in_isSelected)
        drawList->AddText(g_pFntRodin, fontSize, { in_rPosition.x - Scale(35), in_rPosition.y }, IM_COL32_WHITE, ">>");

    drawList->AddText(g_pFntRodin, fontSize, in_rPosition, IM_COL32_WHITE, in_pText);
}

static void DrawDevTitle()
{
    const auto pDevTitleMode = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::DevTitleMode>();

    if (!pDevTitleMode)
        return;

    if (const auto pPictureTitle = StateDevTitle::GetInstance()->m_pPictureTitle.get())
    {
        if (pPictureTitle->m_State != Sonicteam::PictureTitle::PicState_Next)
            return;
    }

    const auto drawList = ImGui::GetBackgroundDrawList();

    const auto& items = pDevTitleMode->m_Modes;
    const auto selectedIndex = pDevTitleMode->m_SelectedIndex.get();

    const auto itemMargin = Scale(80);
    const auto itemHeight = Scale(25);

    drawList->AddRectFilled({ 0, 0 }, { float(Video::s_viewportWidth), float(Video::s_viewportHeight) }, IM_COL32_BLACK);

    DrawInfo();

    for (int i = 0; i < pDevTitleMode->m_Modes.size(); i++)
    {
        const ImVec2 position = { itemMargin, itemMargin + i * itemHeight };

        DrawItem(position, items[i].pName.get(), i == selectedIndex);
    }
}

static void DrawStageSelect()
{
    const auto pStageSelectMode = App::s_pApp->m_pDoc->GetDocMode<Sonicteam::StageSelectMode>();

    if (!pStageSelectMode)
        return;

    const auto drawList = ImGui::GetBackgroundDrawList();

    const auto& items = pStageSelectMode->m_CurrentStageMap->m_Children;
    const auto itemCount = int(items.size());
    const auto selectedIndex = int(pStageSelectMode->m_StageMapIndex.get());

    const auto fontSize = Scale(22);
    const auto itemMargin = Scale(80);
    const auto itemHeight = Scale(25);

    // Ease the scroll offset towards the selection, snapping when the list changes.
    const auto visibleItems = int((Video::s_viewportHeight - (itemMargin * 2.0f)) / itemHeight);
    const auto startOffset = std::max(0, itemCount - visibleItems);
    const auto targetOffset = std::clamp(selectedIndex - visibleItems / 2, 0, startOffset);

    const auto pCurrentStageMap = pStageSelectMode->m_CurrentStageMap.get();

    static Sonicteam::StageMap* pPrevStageMap{};
    static float scrollOffset{};

    if (pCurrentStageMap != pPrevStageMap)
    {
        pPrevStageMap = pCurrentStageMap;
        scrollOffset = targetOffset;
    }
    else
    {
        scrollOffset += (targetOffset - scrollOffset) * (1.0f - expf(-16.0f * App::s_deltaTime));
    }

    DrawInfo();

    drawList->AddText(g_pFntRodin, fontSize, { itemMargin, Scale(56) }, IM_COL32_WHITE, pCurrentStageMap->GetHierarchy().c_str());
    drawList->PushClipRect({ 0, itemMargin }, { float(Video::s_viewportWidth), float(Video::s_viewportHeight) });

    for (int i = 0; i < itemCount; i++)
    {
        const auto& pStageMap = items[i];

        if (!pStageMap)
            continue;

        // The game automatically enters the first child node if only one exists.
        const auto hasChildren = pStageMap->GetTotalChildCount() > 1;

        std::stringstream label{};

        if (hasChildren)
            label << "[";

        label << pStageMap->m_Name.c_str();

        if (hasChildren)
            label << "]";

        const ImVec2 position = { itemMargin, itemMargin + (i - scrollOffset) * itemHeight };

        DrawItem(position, label.str().c_str(), i == selectedIndex);
    }

    drawList->PopClipRect();
}

void DevTitleMenu::Draw()
{
    const auto pApp = App::s_pApp;

    if (!pApp || !pApp->m_pDoc || !pApp->m_pDoc->m_pDocMode)
        return;

    const auto pVftable = pApp->m_pDoc->GetDocMode<Sonicteam::SoX::Engine::Task::Object>()->m_pVftable.ptr.get();

    switch (pVftable)
    {
        // DevTitleMode
        case 0x8202B1E0:
            IsVisible = true;
            DrawDevTitle();
            break;

        // StageSelectMode
        case 0x820336A4:
            IsVisible = true;
            DrawStageSelect();
            break;

        // TODO: handle recovering from TestTextMode and TestCellSpriteMode.
        default:
            IsVisible = false;
            break;
    }
}
