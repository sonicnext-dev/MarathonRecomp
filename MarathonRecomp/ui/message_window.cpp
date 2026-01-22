#include "message_window.h"
#include <api/Marathon.h>
#include <patches/aspect_ratio_patches.h>
#include <gpu/imgui/imgui_snapshot.h>
#include <gpu/video.h>
#include <hid/hid.h>
#include <locale/locale.h>
#include <ui/imgui_utils.h>
#include <app.h>
#include <decompressor.h>
#include <exports.h>
#include <sdl_listener.h>

static bool g_isAwaitingResult{};
static bool g_isClosing{};

static int g_selectedRowIndex{};
static int g_prevSelectedRowIndex{};

static bool g_upWasHeld{};
static bool g_downWasHeld{};

static ImVec2 g_joypadAxis = {};
static bool g_isAccepted{};
static bool g_isDeclined{};

static double g_time{};

static std::string g_text{};
static int g_result{};
static std::vector<std::string> g_buttons{};
static int g_defaultButtonIndex{};
static int g_cancelButtonIndex{};

class SDLEventListenerForMessageWindow : public SDLEventListener
{
public:
    bool OnSDLEvent(SDL_Event* event) override
    {
        if (App::s_isInit || !MessageWindow::s_isVisible || !hid::IsInputAllowed())
            return false;

        constexpr float axisValueRange = 32767.0f;
        constexpr float axisTapRange = 0.5f;

        ImVec2 tapDirection = {};

        switch (event->type)
        {
            case SDL_KEYDOWN:
            {
                switch (event->key.keysym.scancode)
                {
                    case SDL_SCANCODE_UP:
                        g_joypadAxis.y = 1.0f;
                        break;

                    case SDL_SCANCODE_DOWN:
                        g_joypadAxis.y = -1.0f;
                        break;

                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_KP_ENTER:
                        g_isAccepted = true;
                        break;

                    case SDL_SCANCODE_ESCAPE:
                        g_isDeclined = true;
                        break;
                }

                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            {
                // Only accept left mouse button.
                if (event->button.button != SDL_BUTTON_LEFT)
                    break;

                // Only accept mouse buttons when an item is selected.
                if (g_selectedRowIndex == -1)
                    break;

                g_isAccepted = true;

                break;
            }

            case SDL_CONTROLLERBUTTONDOWN:
            {
                switch (event->cbutton.button)
                {
                    case SDL_CONTROLLER_BUTTON_DPAD_UP:
                        g_joypadAxis = { 0.0f, 1.0f };
                        break;

                    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                        g_joypadAxis = { 0.0f, -1.0f };
                        break;

                    case SDL_CONTROLLER_BUTTON_A:
                        g_isAccepted = true;
                        break;

                    case SDL_CONTROLLER_BUTTON_B:
                        g_isDeclined = true;
                        break;
                }

                break;
            }

            case SDL_CONTROLLERAXISMOTION:
            {
                if (event->caxis.axis < 2)
                {
                    float newAxisValue = -(event->caxis.value / axisValueRange);
                    bool sameDirection = (newAxisValue * g_joypadAxis[event->caxis.axis]) > 0.0f;
                    bool wasInRange = abs(g_joypadAxis[event->caxis.axis]) > axisTapRange;
                    bool isInRange = abs(newAxisValue) > axisTapRange;

                    if (sameDirection && !wasInRange && isInRange)
                        tapDirection[event->caxis.axis] = newAxisValue;

                    g_joypadAxis[event->caxis.axis] = newAxisValue;
                }

                break;
            }
        }

        return false;
    }
}
g_sdlEventListenerForMessageWindow;

void DrawButton(int rowIndex, float yOffset, float yPadding, float width, float height, std::string& text)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    auto clipRectMin = drawList->GetClipRectMin();
    auto clipRectMax = drawList->GetClipRectMax();

    ImVec2 min = { clipRectMin.x + ((clipRectMax.x - clipRectMin.x) - width) / 2, clipRectMin.y + (height * rowIndex) + yOffset + (yPadding * rowIndex) };
    ImVec2 max = { min.x + width, min.y + height };

    auto textColour = IM_COL32_WHITE;

    if (rowIndex == g_selectedRowIndex)
    {
        auto gb = 255 * BREATHE_MOTION(1.0f, 0.0f, g_time, (g_isClosing ? 0.1f : 0.9f));

        textColour = IM_COL32(255, gb, gb, 255);

        if (!g_isClosing)
            DrawArrowCursor(min, g_time, false, true);
    }

    auto fontSize = Scale(27, true);
    auto textSize = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0, text.c_str());

    // Show low quality text in-game.
    if (App::s_isInit)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);

    DrawTextBasic
    (
        g_pFntRodin,
        fontSize,
        { /* X */ min.x + ((max.x - min.x) - textSize.x) / 2, /* Y */ min.y + ((max.y - min.y) - textSize.y) / 2 },
        textColour,
        text.c_str()
    );

    // Reset the shader modifier.
    if (App::s_isInit)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_NONE);
}

static void ResetSelection()
{
    /* Always use -1 for mouse input to prevent the selection
       cursor from erroneously appearing where it shouldn't. */
    g_selectedRowIndex = hid::g_inputDevice == hid::EInputDevice::Mouse
        ? -1
        : g_defaultButtonIndex;

    g_upWasHeld = false;
    g_downWasHeld = false;
    g_joypadAxis = {};
    g_isAccepted = false;
    g_isDeclined = false;
}

void MessageWindow::Draw()
{
    if (!s_isVisible)
        return;

    if (g_isClosing && (ImGui::GetTime() - g_time) > (1.0 / 60.0) * 30.0)
    {
        g_isAwaitingResult = false;
        s_isVisible = false;
        return;
    }

    auto drawList = ImGui::GetBackgroundDrawList();
    auto isController = hid::IsInputDeviceController();
    auto isKeyboard = hid::g_inputDevice == hid::EInputDevice::Keyboard;

    // Handle controller input when the game is booted.
    if (App::s_isInit)
    {
        // Always assume keyboard to prevent mouse from blocking control in-game.
        isKeyboard = true;

        if (auto& spInputManager = App::s_pApp->m_pDoc->m_vspInputManager[0])
        {
            auto& rPadState = spInputManager->m_PadState;

            g_joypadAxis.y = -rPadState.LeftStickVertical;

            if (rPadState.IsPressed(Sonicteam::SoX::Input::KeyState_DPadUp))
                g_joypadAxis.y = 1.0f;

            if (rPadState.IsPressed(Sonicteam::SoX::Input::KeyState_DPadDown))
                g_joypadAxis.y = -1.0f;

            g_isAccepted = rPadState.IsPressed(Sonicteam::SoX::Input::KeyState_A);
            g_isDeclined = rPadState.IsPressed(Sonicteam::SoX::Input::KeyState_B);

            if (isKeyboard)
                g_isAccepted = g_isAccepted || rPadState.IsPressed(Sonicteam::SoX::Input::KeyState_Start);
        }
    }

    ImVec2 msgMin = { g_horzCentre + Scale(96, true), g_vertCentre + Scale(96, true) };
    ImVec2 msgMax = { msgMin.x + Scale(1088, true), msgMin.y + Scale(384, true) };
    ImVec2 msgCentre = { (msgMin.x / 2) + (msgMax.x / 2), (msgMin.y / 2) + (msgMax.y / 2) };

    DrawWindow(msgMin, msgMax);

    // Use low quality text when the game is booted to not clash with existing UI.
    if (App::s_isInit)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);

    auto fontSize = Scale(27, true);
    auto textSize = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0, g_text.c_str());

    DrawTextBasic(g_pFntRodin, fontSize, { msgCentre.x - textSize.x / 2, msgCentre.y - textSize.y / 2 }, IM_COL32_WHITE, g_text.c_str());

    // Reset the shader modifier.
    if (App::s_isInit)
        SetShaderModifier(IMGUI_SHADER_MODIFIER_LOW_QUALITY_TEXT);

    drawList->PopClipRect();

    ImVec2 selMin = { msgMin.x, msgMax.y + ((msgMin.y - g_vertCentre) / 2) };
    ImVec2 selMax = { msgMax.x, selMin.y + Scale(128, true) };

    DrawWindow(selMin, selMax);

    auto rowCount = 0;
    auto windowMarginX = Scale(36, true);
    auto itemWidth = msgMax.x - msgMin.x - windowMarginX;
    auto itemHeight = Scale(25, true);
    auto itemPadding = Scale(18, true);
    auto windowMarginY = ((selMax.y - selMin.y) / 2) - (((itemHeight + itemPadding) / 2) * g_buttons.size());

    for (auto& button : g_buttons)
        DrawButton(rowCount++, windowMarginY, itemPadding, itemWidth, itemHeight, button);

    if (isController || isKeyboard)
    {
        auto upIsHeld = g_joypadAxis.y > 0.5f;
        auto downIsHeld = g_joypadAxis.y < -0.5f;
        auto scrollUp = !g_upWasHeld && upIsHeld;
        auto scrollDown = !g_downWasHeld && downIsHeld;
            
        auto prevSelectedRowIndex = g_selectedRowIndex;
            
        if (scrollUp)
        {
            if (g_selectedRowIndex > 0)
                --g_selectedRowIndex;
        }
        else if (scrollDown)
        {
            if (g_selectedRowIndex < rowCount - 1)
                ++g_selectedRowIndex;
        }
            
        if (scrollUp || scrollDown)
        {
            Game_PlaySound("move");
            g_prevSelectedRowIndex = prevSelectedRowIndex;
            g_joypadAxis.y = 0;
        }
            
        g_upWasHeld = upIsHeld;
        g_downWasHeld = downIsHeld;
            
        if (g_isDeclined)
        {
            if (g_buttons.size() == 1)
            {
                Game_PlaySound("window_close");
            }
            else
            {
                if (g_selectedRowIndex == g_cancelButtonIndex)
                {
                    Game_PlaySound("window_close");
                }
                else
                {
                    Game_PlaySound("move");
                }

                g_selectedRowIndex = g_cancelButtonIndex;
            }

            g_isDeclined = false;
        }
    }
    else if (!App::s_isInit) // Only accept mouse input during installer.
    {
        auto clipRectMin = drawList->GetClipRectMin();
        auto clipRectMax = drawList->GetClipRectMax();

        ImVec2 listMin = { clipRectMin.x, clipRectMin.y + windowMarginY };
        ImVec2 listMax = { clipRectMax.x, clipRectMin.y + windowMarginY + (itemHeight * rowCount) + (itemPadding * rowCount) };

        // Invalidate index if the mouse cursor is outside of the list box.
        if (!ImGui::IsMouseHoveringRect(listMin, listMax, false))
            g_selectedRowIndex = -1;

        for (int i = 0; i < rowCount; i++)
        {
            auto currentHeight = itemHeight * i;
            auto currentPadding = itemPadding * i;

            ImVec2 itemMin = { listMin.x, listMin.y + currentHeight + currentPadding };
            ImVec2 itemMax = { listMax.x, clipRectMin.y + windowMarginY + currentHeight + itemHeight + currentPadding };

            if (ImGui::IsMouseHoveringRect(itemMin, itemMax, false))
            {
                if (g_selectedRowIndex != i)
                    Game_PlaySound("move");

                g_selectedRowIndex = i;

                break;
            }
        }
    }

    if (g_selectedRowIndex != -1 && g_isAccepted && !g_isClosing)
    {
        g_result = g_selectedRowIndex;

        Game_PlaySound("main_deside");

        MessageWindow::Close();
    }

    drawList->PopClipRect();
}

bool MessageWindow::Open(std::string text, int* result, std::span<std::string> buttons, int defaultButtonIndex, int cancelButtonIndex)
{
    if (!g_isAwaitingResult && *result == -1)
    {
        s_isVisible = true;
        g_isClosing = false;
        g_time = ImGui::GetTime();

        g_text = text;
        g_buttons = std::vector(buttons.begin(), buttons.end());
        g_defaultButtonIndex = defaultButtonIndex;
        g_cancelButtonIndex = cancelButtonIndex;

        if (g_buttons.empty())
            g_buttons.push_back(Localise("Common_OK"));

        ResetSelection();

        g_isAwaitingResult = true;
    }
    
    *result = g_result;

    // Returns true when the message window is closed.
    return !g_isAwaitingResult;
}

void MessageWindow::Close()
{
    if (g_isClosing)
        return;

    g_time = ImGui::GetTime();
    g_isClosing = true;
}
