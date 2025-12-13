#include "installer_wizard.h"

#include <nfd.h>

#include <apu/embedded_player.h>
#include <install/installer.h>
#include <gpu/video.h>
#include <gpu/imgui/imgui_snapshot.h>
#include <hid/hid.h>
#include <locale/locale.h>
#include <patches/aspect_ratio_patches.h>
#include <ui/imgui_utils.h>
#include <ui/button_window.h>
#include <ui/common_menu.h>
#include <ui/message_window.h>
#include <ui/game_window.h>
#include <decompressor.h>
#include <exports.h>
#include <sdl_listener.h>

#include <res/images/common/libertyrecomp.dds.h>
#include <res/images/installer/install_001.dds.h>
#include <res/images/installer/install_002.dds.h>
#include <res/images/installer/install_003.dds.h>
#include <res/images/installer/install_004.dds.h>
#include <res/images/installer/install_005.dds.h>
#include <res/images/installer/install_006.dds.h>
#include <res/images/installer/install_007.dds.h>
#include <res/images/installer/install_008.dds.h>
#include <res/credits.h>

// One Shot Animations Constants
static constexpr double SCANLINES_ANIMATION_TIME = 0.0;
static constexpr double SCANLINES_ANIMATION_DURATION = 15.0;

static constexpr double IMAGE_ANIMATION_TIME = SCANLINES_ANIMATION_TIME + SCANLINES_ANIMATION_DURATION;
static constexpr double IMAGE_ANIMATION_DURATION = 15.0;

static constexpr double TITLE_ANIMATION_TIME = SCANLINES_ANIMATION_DURATION;
static constexpr double TITLE_ANIMATION_DURATION = 30.0;

static constexpr double CONTAINER_LINE_ANIMATION_TIME = SCANLINES_ANIMATION_DURATION;
static constexpr double CONTAINER_LINE_ANIMATION_DURATION = 23.0;

static constexpr double CONTAINER_INNER_TIME = SCANLINES_ANIMATION_DURATION + CONTAINER_LINE_ANIMATION_DURATION + 8.0;
static constexpr double CONTAINER_INNER_DURATION = 15.0;

static constexpr double ALL_ANIMATIONS_FULL_DURATION = CONTAINER_INNER_TIME + CONTAINER_INNER_DURATION;
static constexpr double QUITTING_EXTRA_DURATION = 60.0;

constexpr float IMAGE_WIDTH = 1000.0f;
constexpr float IMAGE_HEIGHT = 1000.0f;

constexpr float CONTAINER_X = 650.0f;
constexpr float CONTAINER_Y = 226.0f;
constexpr float CONTAINER_WIDTH = 526.5f;
constexpr float CONTAINER_HEIGHT = 246.0f;
constexpr float CONTAINER_PADDING = 25.0f;
constexpr float SIDE_CONTAINER_WIDTH = CONTAINER_WIDTH / 2.0f;

constexpr float BOTTOM_X_GAP = 4.0f;
constexpr float BOTTOM_Y_GAP = 5.0f;
constexpr float CONTAINER_BUTTON_WIDTH = 250.0f;
constexpr float CONTAINER_BUTTON_GAP = 9.0f;
constexpr float BUTTON_HEIGHT = 22.0f;
constexpr float BUTTON_TEXT_GAP = 28.0f;

constexpr float BORDER_SIZE = 1.0f;
constexpr float BORDER_OVERSHOOT = 36.0f;

static constexpr size_t GRID_SIZE = 9;

static CommonMenu g_commonMenu;

static double g_chevronTime = 0.0;
static double g_appearTime = 0.0;
static double g_disappearTime = DBL_MAX;
static bool g_isDisappearing = false;
static bool g_isQuitting = false;

static std::filesystem::path g_installPath;
static std::filesystem::path g_gameSourcePath;
static std::array<std::filesystem::path, int(DLC::Count)> g_dlcSourcePaths;
static std::array<bool, int(DLC::Count)> g_dlcInstalled = {};
static std::array<std::unique_ptr<GuestTexture>, 8> g_installTextures;
static std::unique_ptr<GuestTexture> g_upLibertyDev;
static Journal g_installerJournal;
static Installer::Sources g_installerSources;
static uint64_t g_installerAvailableSize = 0;
static std::unique_ptr<std::thread> g_installerThread;
static double g_installerStartTime = 0.0;
static double g_installerEndTime = DBL_MAX;
static float g_installerProgressRatioCurrent = 0.0f;
static std::atomic<float> g_installerProgressRatioTarget = 0.0f;
static std::atomic<bool> g_installerFinished = false;
static std::atomic<bool> g_installerHalted = false;
static std::atomic<bool> g_installerCancelled = false;
static bool g_installerFailed = false;
static std::string g_installerErrorMessage;

static std::array<ImVec2, 8> g_installTexturePositions = { ImVec2(120.0f, 90.0f),    // Sonic
                                                           ImVec2(100.0f, 80.0f),    // Tails
                                                           ImVec2(120.0f, 90.0f),    // Amy
                                                           ImVec2(10.0f, 90.0f),     // Shadow
                                                           ImVec2(120.0f, 90.0f),    // Rouge
                                                           ImVec2(120.0f, 10.0f),    // Silver
                                                           ImVec2(10.0f, 140.0f),    // Eggman
                                                           ImVec2(200.0f, 140.0f) }; // Elise

enum class WizardPage
{
    SelectLanguage,
    Introduction,
    SelectGame,
    SelectDLC,
    CheckSpace,
    Installing,
    InstallSucceeded,
    InstallFailed,
};

enum class MessagePromptSource
{
    Unknown,
    Next,
    Back
};

static WizardPage g_firstPage = WizardPage::SelectLanguage;
static WizardPage g_currentPage = g_firstPage;
static std::string g_currentMessagePrompt = "";
static MessagePromptSource g_currentMessagePromptSource = MessagePromptSource::Unknown;
static bool g_currentMessagePromptConfirmation = false;
static std::list<std::filesystem::path> g_currentPickerResults;
static std::atomic<bool> g_currentPickerResultsReady = false;
static std::string g_currentPickerErrorMessage;
static std::unique_ptr<std::thread> g_currentPickerThread;
static bool g_pickerTutorialCleared[2] = {};
static bool g_pickerTutorialTriggered = false;
static bool g_pickerTutorialFolderMode = false;
static bool g_currentPickerVisible = false;
static bool g_currentPickerFolderMode = false;
static int g_currentMessageResult = -1;
static ImVec2 g_joypadAxis = {};
static int g_currentCursorIndex = -1;
static int g_currentCursorDefault = 0;
static bool g_currentCursorAccepted = false;
static bool g_currentCursorBack = false;
static std::vector<std::pair<ImVec2, ImVec2>> g_currentCursorRects;
static std::string g_creditsStr;

class SDLEventListenerForInstaller : public SDLEventListener
{
public:
    bool OnSDLEvent(SDL_Event *event) override
    {
        if (!InstallerWizard::s_isVisible)
            return false;

        bool noModals = g_currentMessagePrompt.empty() && !g_currentPickerVisible;
        if (event->type == SDL_QUIT && g_currentPage == WizardPage::Installing)
        {
            // Pretend the back button was pressed if the user tried quitting during installation.
            // This condition is above the rest of the event processing as we want to block the exit
            // button while there's confirmation message is open as well.
            if (noModals)
                g_currentCursorBack = true;

            return true;
        }

        if (!noModals || !hid::IsInputAllowed())
            return false;

        constexpr float AxisValueRange = 32767.0f;
        constexpr float AxisTapRange = 0.5f;
        int newCursorIndex = -1;
        ImVec2 tapDirection = {};

        switch (event->type)
        {
            case SDL_KEYDOWN:
            {
                switch (event->key.keysym.scancode)
                {
                    case SDL_SCANCODE_LEFT:
                    case SDL_SCANCODE_RIGHT:
                        tapDirection.x = (event->key.keysym.scancode == SDL_SCANCODE_RIGHT) ? 1.0f : -1.0f;
                        break;
                    case SDL_SCANCODE_UP:
                    case SDL_SCANCODE_DOWN:
                        tapDirection.y = (event->key.keysym.scancode == SDL_SCANCODE_DOWN) ? 1.0f : -1.0f;
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_KP_ENTER:
                        g_currentCursorAccepted = (g_currentCursorIndex >= 0);
                        break;
                    case SDL_SCANCODE_ESCAPE:
                        g_currentCursorBack = true;
                        break;
                }

                break;
            }

            case SDL_CONTROLLERBUTTONDOWN:
            {
                switch (event->cbutton.button)
                {
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                        tapDirection = { -1.0f, 0.0f };
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                        tapDirection = { 1.0f, 0.0f };
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_UP:
                        tapDirection = { 0.0f, -1.0f };
                        break;
                    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                        tapDirection = { 0.0f, 1.0f };
                        break;
                    case SDL_CONTROLLER_BUTTON_A:
                        g_currentCursorAccepted = (g_currentCursorIndex >= 0);
                        break;
                    case SDL_CONTROLLER_BUTTON_B:
                        g_currentCursorBack = true;
                        break;
                }

                break;
            }

            case SDL_CONTROLLERAXISMOTION:
            {
                if (event->caxis.axis < 2)
                {
                    float newAxisValue = event->caxis.value / AxisValueRange;
                    bool sameDirection = (newAxisValue * g_joypadAxis[event->caxis.axis]) > 0.0f;
                    bool wasInRange = abs(g_joypadAxis[event->caxis.axis]) > AxisTapRange;
                    bool isInRange = abs(newAxisValue) > AxisTapRange;
                    if (sameDirection && !wasInRange && isInRange)
                    {
                        tapDirection[event->caxis.axis] = newAxisValue;
                    }

                    g_joypadAxis[event->caxis.axis] = newAxisValue;
                }

                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEMOTION:
            {
                for (size_t i = 0; i < g_currentCursorRects.size(); i++)
                {
                    auto &currentRect = g_currentCursorRects[i];

                    if (ImGui::IsMouseHoveringRect(currentRect.first, currentRect.second, false))
                    {
                        newCursorIndex = int(i);

                        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
                            g_currentCursorAccepted = true;

                        break;
                    }
                }

                if (newCursorIndex < 0)
                    g_currentCursorIndex = -1;

                break;
            }
        }

        if (tapDirection.x != 0.0f || tapDirection.y != 0.0f)
        {
            if (g_currentCursorIndex >= g_currentCursorRects.size() || g_currentCursorIndex < 0)
            {
                newCursorIndex = g_currentCursorDefault;
            }
            else
            {
                auto &currentRect = g_currentCursorRects[g_currentCursorIndex];
                ImVec2 currentPoint = ImVec2
                (
                    (currentRect.first.x + currentRect.second.x) / 2.0f + tapDirection.x * (currentRect.second.x - currentRect.first.x) / 2.0f,
                    (currentRect.first.y + currentRect.second.y) / 2.0f + tapDirection.y * (currentRect.second.y - currentRect.first.y) / 2.0f
                );

                float closestDistance = FLT_MAX;
                for (size_t i = 0; i < g_currentCursorRects.size(); i++)
                {
                    if (g_currentCursorIndex == i)
                    {
                        continue;
                    }

                    auto &targetRect = g_currentCursorRects[i];
                    ImVec2 targetPoint = ImVec2
                    (
                        (targetRect.first.x + targetRect.second.x) / 2.0f + tapDirection.x * (targetRect.first.x - targetRect.second.x) / 2.0f,
                        (targetRect.first.y + targetRect.second.y) / 2.0f + tapDirection.y * (targetRect.first.y - targetRect.second.y) / 2.0f
                    );

                    ImVec2 delta = ImVec2(targetPoint.x - currentPoint.x, targetPoint.y - currentPoint.y);
                    float projectedDistance = delta.x * tapDirection.x + delta.y * tapDirection.y;
                    float manhattanDistance = abs(delta.x) + abs(delta.y);
                    if (projectedDistance > 0.0f && manhattanDistance < closestDistance)
                    {
                        newCursorIndex = int(i);
                        closestDistance = manhattanDistance;
                    }
                }
            }
        }

        if (newCursorIndex >= 0)
        {
            if (g_currentCursorIndex != newCursorIndex)
                Game_PlaySound("move");

            g_currentCursorIndex = newCursorIndex;
        }

        return false;
    }
}
g_sdlEventListenerForInstaller;

static std::string& GetWizardText(WizardPage page)
{
    switch (page)
    {
        case WizardPage::SelectLanguage: return Localise("Installer_Page_SelectLanguage");
        case WizardPage::Introduction: return Localise("Installer_Page_Introduction");
        case WizardPage::SelectGame: return Localise("Installer_Page_SelectGame");
        case WizardPage::SelectDLC: return Localise("Installer_Page_SelectDLC");
        case WizardPage::CheckSpace: return Localise("Installer_Page_CheckSpace");
        case WizardPage::Installing: return Localise("Installer_Page_Installing");
        case WizardPage::InstallSucceeded: return Localise("Installer_Page_InstallSucceeded");
        case WizardPage::InstallFailed: return Localise("Installer_Page_InstallFailed");
    }

    return g_localeMissing;
}

static const int WIZARD_INSTALL_TEXTURE_INDEX[] =
{
    0,
    1,
    2,
    3,
    4,
    5,
    7, // Force Elise on InstallSucceeded.
    6  // Force Eggman on InstallFailed.
};

// These are ordered from bottom to top in a 3x2 grid.
const char* LANGUAGE_TEXT[] =
{
    "FRANÇAIS", // French
    "DEUTSCH", // German
    "ENGLISH", // English
    "ESPAÑOL", // Spanish
    "ITALIANO", // Italian
    "日本語", // Japanese
};

const ELanguage LANGUAGE_ENUM[] =
{
    ELanguage::French,
    ELanguage::German,
    ELanguage::English,
    ELanguage::Spanish,
    ELanguage::Italian,
    ELanguage::Japanese,
};

const char *DLC_SOURCE_TEXT[] =
{
    "SONIC BOSS ATTACK",
    "SHADOW BOSS ATTACK",
    "SILVER BOSS ATTACK",
    "TEAM ATTACK AMIGO",
    "SONIC VERY HARD",
    "SHADOW VERY HARD",
    "SILVER VERY HARD",
};

static int DLCIndex(DLC dlc)
{
    assert(dlc != DLC::Unknown);
    return (int)(dlc) - 1;
}

static void SetCurrentPage(WizardPage page)
{
    g_currentPage = page;

    if (g_currentPage == WizardPage::InstallSucceeded)
    {
        ButtonWindow::Open("Button_Select");
    }
    else if (g_currentPage != WizardPage::Installing)
    {
        auto key = "Button_SelectBack";

        if (g_currentPage == g_firstPage || g_currentPage == WizardPage::InstallFailed)
            key = "Button_SelectQuit";

        ButtonWindow::Open(key);
    }
    else if (g_currentPage == WizardPage::Installing)
    {
        ButtonWindow::Open("Button_Cancel");
    }
    else
    {
        ButtonWindow::Close();
    }
}

static double ComputeMotionInstaller(double timeAppear, double timeDisappear, double offset, double total)
{
    return ComputeMotion(timeAppear, offset, total) * (1.0 - ComputeMotion(timeDisappear, ALL_ANIMATIONS_FULL_DURATION - offset - total, total));
}

static bool PushCursorRect(ImVec2 min, ImVec2 max, bool &cursorPressed, bool makeDefault = false)
{
    int currentIndex = int(g_currentCursorRects.size());
    g_currentCursorRects.emplace_back(min, max);

    if (makeDefault)
    {
        g_currentCursorDefault = currentIndex;
    }

    if (g_currentCursorIndex == currentIndex)
    {
        if (g_currentCursorAccepted)
        {
            Game_PlaySound("main_deside");
            cursorPressed = true;
            g_currentCursorAccepted = false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

static void ResetCursorRects()
{
    g_currentCursorDefault = 0;
    g_currentCursorRects.clear();
}

static void DrawBackground()
{
    auto &res = ImGui::GetIO().DisplaySize;
    auto drawList = ImGui::GetBackgroundDrawList();

    const uint32_t TOP = IM_COL32(0, 103, 255, 255);
    const uint32_t BOTTOM = IM_COL32(0, 40, 100, 255);

    drawList->AddRectFilledMultiColor({ 0.0, 0.0 }, res, TOP, TOP, BOTTOM, BOTTOM);
}

static void DrawArrows()
{
    auto& res = ImGui::GetIO().DisplaySize;

    DrawArrows({ 0, 0 }, res, g_chevronTime);
}

static void DrawLeftImage()
{
    int installTextureIndex = WIZARD_INSTALL_TEXTURE_INDEX[int(g_currentPage)];
    if (g_currentPage == WizardPage::Installing)
    {
        // Cycle through the available images while time passes during installation.
        constexpr double InstallationSpeed = 1.0 / 15.0;
        double installationTime = (ImGui::GetTime() - g_installerStartTime) * InstallationSpeed;
        installTextureIndex += int(installationTime);
    }

    double imageAlpha = ComputeMotionInstaller(g_appearTime, g_disappearTime, IMAGE_ANIMATION_TIME, IMAGE_ANIMATION_DURATION);
    int a = std::lround(140.0 * imageAlpha);
    GuestTexture *guestTexture = g_installTextures[installTextureIndex % g_installTextures.size()].get();
    auto &res = ImGui::GetIO().DisplaySize;
    auto drawList = ImGui::GetBackgroundDrawList();
    auto pos = g_installTexturePositions[installTextureIndex % g_installTextures.size()];
    ImVec2 min = { g_aspectRatioOffsetX + Scale(pos.x), g_aspectRatioOffsetY + Scale(pos.y) };
    ImVec2 max = { min.x + Scale(IMAGE_WIDTH), min.y + Scale(IMAGE_HEIGHT) };
    drawList->AddImage(guestTexture, min, max, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, a));
}

static void DrawContainer(ImVec2 min, ImVec2 max, bool isTextArea)
{   
    auto &res = ImGui::GetIO().DisplaySize;
    auto drawList = ImGui::GetBackgroundDrawList();

    double gridOverlayAlpha = ComputeMotionInstaller(g_appearTime, g_disappearTime, CONTAINER_INNER_TIME, CONTAINER_INNER_DURATION);

    if (isTextArea)
    {
        auto padding = Scale(CONTAINER_PADDING);

        DrawContainerBox({ min.x - padding, min.y - padding }, { max.x + padding, max.y + padding }, gridOverlayAlpha);
    }

    float gridSize = Scale(GRID_SIZE);

    // The draw area
    drawList->PushClipRect({ min.x - gridSize * 2.0f, min.y + gridSize * 2.0f }, { max.x - gridSize * 2.0f + 1.0f, max.y - gridSize * 2.0f + 1.0f });
}

static void DrawDescriptionContainer()
{
    auto &res = ImGui::GetIO().DisplaySize;
    auto drawList = ImGui::GetBackgroundDrawList();
    auto fontSize = Scale(25.0f);

    ImVec2 descriptionMin = { round(g_aspectRatioOffsetX + Scale(CONTAINER_X + 0.5f)), round(g_aspectRatioOffsetY + Scale(CONTAINER_Y + 0.5f)) };
    ImVec2 descriptionMax = { round(g_aspectRatioOffsetX + Scale(CONTAINER_X + 0.5f + CONTAINER_WIDTH)), round(g_aspectRatioOffsetY + Scale(CONTAINER_Y + 0.5f + CONTAINER_HEIGHT)) };
    SetProceduralOrigin(descriptionMin);
    DrawContainer(descriptionMin, descriptionMax, true);

    char descriptionText[512];
    char requiredSpaceText[128];
    char availableSpaceText[128];
    strncpy(descriptionText, GetWizardText(g_currentPage).c_str(), sizeof(descriptionText) - 1);

    if (g_currentPage == WizardPage::CheckSpace)
    {
        constexpr double DivisorGiB = (1024.0 * 1024.0 * 1024.0);
        double requiredGiB = double(g_installerSources.totalSize) / DivisorGiB;
        double availableGiB = double(g_installerAvailableSize) / DivisorGiB;
        snprintf(requiredSpaceText, sizeof(requiredSpaceText), Localise("Installer_Step_RequiredSpace").c_str(), requiredGiB);
        snprintf(availableSpaceText, sizeof(availableSpaceText), (g_installerAvailableSize > 0) ? Localise("Installer_Step_AvailableSpace").c_str() : "", availableGiB);
        snprintf(descriptionText, sizeof(descriptionText), "%s%s\n%s", GetWizardText(g_currentPage).c_str(), requiredSpaceText, availableSpaceText);
    }
    else if (g_currentPage == WizardPage::InstallFailed)
    {
        // Japanese needs text to be brought in by a normal width space
        // as it allows for text to begin further than others for
        // special characters.
        if (Config::Language == ELanguage::Japanese)
        {
            strncat(descriptionText, " ", 1);
        }

        strncat(descriptionText, g_installerErrorMessage.c_str(), sizeof(descriptionText) - 1);
    }

    double textAlpha = ComputeMotionInstaller(g_appearTime, g_disappearTime, CONTAINER_INNER_TIME, CONTAINER_INNER_DURATION);
    auto clipRectMin = drawList->GetClipRectMin();
    auto clipRectMax = drawList->GetClipRectMax();

    float textX = clipRectMin.x + fontSize;
    float textY = clipRectMin.y - Scale(1.0f);

    auto lineWidth = clipRectMax.x - (fontSize / 2.0f) - clipRectMin.x;

    clipRectMax.x += fontSize;
    clipRectMax.y += Scale(1.0f);

    float lineMargin = 5.0f;

    drawList->PushClipRect(clipRectMin, clipRectMax, false);

    DrawTextParagraph
    (
        g_pFntRodin,
        fontSize,
        lineWidth,
        { textX, textY },
        lineMargin,
        descriptionText,

        [=](const char* str, ImVec2 pos)
        {
            DrawTextBasic(g_pFntRodin, fontSize, pos, IM_COL32(255, 255, 255, 255 * textAlpha), str);
        }
    );

    drawList->PopClipRect();
    drawList->PopClipRect();

    if (g_currentPage == WizardPage::InstallSucceeded)
    {
        auto descTextSize = MeasureCentredParagraph(g_pFntRodin, fontSize, lineWidth, lineMargin, descriptionText);

        auto colWhite = IM_COL32(255, 255, 255, 255 * textAlpha);

        auto containerLeft = g_aspectRatioOffsetX + Scale(CONTAINER_X);
        auto containerTop = g_aspectRatioOffsetY + Scale(CONTAINER_Y);
        auto containerRight = containerLeft + Scale(CONTAINER_WIDTH);
        auto containerBottom = containerTop + Scale(CONTAINER_HEIGHT);

        auto marqueeTextSize = g_pFntRodin->CalcTextSizeA(fontSize, FLT_MAX, 0, g_creditsStr.c_str());
        auto marqueeTextMarginX = Scale(5);
        auto marqueeTextMarginY = Scale(15);

        ImVec2 marqueeTextPos = { descriptionMax.x, containerBottom - marqueeTextSize.y - marqueeTextMarginY };
        ImVec2 marqueeTextMin = { containerLeft, marqueeTextPos.y };
        ImVec2 marqueeTextMax = { containerRight, containerBottom };

        auto imageWidth = Scale(524);
        auto imageHeight = Scale(45);

        ImVec2 imageRegionMin = { containerLeft, textY + descTextSize.y };
        ImVec2 imageRegionMax = { containerRight, containerBottom - (marqueeTextMax.y - marqueeTextMin.y) };

        ImVec2 imageMin = 
        {
            /* X */ imageRegionMin.x + ((imageRegionMax.x - imageRegionMin.x) / 2) - (imageWidth / 2),
            /* Y */ imageRegionMin.y + ((imageRegionMax.y - imageRegionMin.y) / 2) - (imageHeight / 2)
        };

        ImVec2 imageMax = { imageMin.x + imageWidth, imageMin.y + imageHeight };

        drawList->AddImage(g_upLibertyDev.get(), imageMin, imageMax, { 0, 0 }, { 1, 1 }, colWhite);

        // SetHorizontalMarqueeFade(marqueeTextMin, marqueeTextMax, Scale(32));
        // DrawTextWithMarquee(g_pFntRodin, fontSize, marqueeTextPos, marqueeTextMin, marqueeTextMax, colWhite, g_creditsStr.c_str(), g_installerEndTime, 0.9, Scale(200));
        // ResetMarqueeFade();
    }

    ImVec2 sideMin = { descriptionMax.x, descriptionMin.y };
    ImVec2 sideMax = { res.x, descriptionMax.y };

    DrawContainer(sideMin, sideMax, false);

    drawList->PopClipRect();

    ResetProceduralOrigin();
}

static void DrawButtonContainer(ImVec2 min, ImVec2 max, int baser, int baseg, float alpha)
{
    auto &res = ImGui::GetIO().DisplaySize;
    auto drawList = ImGui::GetBackgroundDrawList();

    drawList->AddRectFilledMultiColor(min, max, IM_COL32(baser, baseg + 130, 0, 223 * alpha), IM_COL32(baser, baseg + 130, 0, 178 * alpha), IM_COL32(baser, baseg + 130, 0, 223 * alpha), IM_COL32(baser, baseg + 130, 0, 178 * alpha));
    drawList->AddRectFilledMultiColor(min, max, IM_COL32(baser, baseg, 0, 13 * alpha), IM_COL32(baser, baseg, 0, 0), IM_COL32(baser, baseg, 0, 55 * alpha), IM_COL32(baser, baseg, 0, 6 * alpha));
    drawList->AddRectFilledMultiColor(min, max, IM_COL32(baser, baseg + 130, 0, 13 * alpha), IM_COL32(baser, baseg + 130, 0, 111 * alpha), IM_COL32(baser, baseg + 130, 0, 0), IM_COL32(baser, baseg + 130, 0, 55 * alpha));
}

static ImVec2 ComputeTextSize(ImFont *font, const char *text, float size, float &squashRatio, float maxTextWidth = FLT_MAX)
{
    ImVec2 textSize = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text);
    if (textSize.x > maxTextWidth)
    {
        squashRatio = maxTextWidth / textSize.x;
    }
    else
    {
        squashRatio = 1.0f;
    }

    return textSize;
}

static void DrawButton(ImVec2 min, ImVec2 max, const char *buttonText, bool sourceButton, bool buttonEnabled, bool &buttonPressed, float maxTextWidth = FLT_MAX, bool makeDefault = false)
{
    buttonPressed = false;

    auto &res = ImGui::GetIO().DisplaySize;
    auto drawList = ImGui::GetBackgroundDrawList();
    float alpha = ComputeMotionInstaller(g_appearTime, g_disappearTime, CONTAINER_INNER_TIME, CONTAINER_INNER_DURATION);
    if (!buttonEnabled)
    {
        alpha *= 0.5f;
    }

    int baser = 0;
    int baseg = 0;
    if (g_currentMessagePrompt.empty() && !g_currentPickerVisible && !sourceButton && buttonEnabled && (alpha >= 1.0f))
    {
        bool cursorOnButton = PushCursorRect(min, max, buttonPressed, makeDefault);
        if (cursorOnButton)
        {
            baser = 48;
            baseg = 32;
        }
    }

    DrawButtonContainer(min, max, baser, baseg, alpha);

    ImFont *font = g_pFntRodin;
    float size = Scale(18.0f);
    float squashRatio;
    ImVec2 textSize = ComputeTextSize(font, buttonText, size, squashRatio, Scale(maxTextWidth));
    min.x += ((max.x - min.x) - textSize.x) / 2.0f;
    min.y += ((max.y - min.y) - textSize.y) / 2.0f;

    SetOrigin({ min.x + textSize.x / 2.0f, min.y });
    SetScale({ squashRatio, 1.0f });

    DrawTextBasic
    (
        font,
        size,
        min,
        IM_COL32(255, 255, 255, 255 * alpha),
        buttonText
    );

    SetScale({ 1.0f, 1.0f });
    SetOrigin({ 0.0f, 0.0f });
}

enum ButtonColumn
{
    ButtonColumnLeft,
    ButtonColumnMiddle,
    ButtonColumnRight
};

static void ComputeButtonColumnCoordinates(ButtonColumn buttonColumn, float &minX, float &maxX)
{
    switch (buttonColumn)
    {
    case ButtonColumnLeft:
        minX = g_aspectRatioOffsetX + Scale(CONTAINER_X + CONTAINER_BUTTON_GAP);
        maxX = g_aspectRatioOffsetX + Scale(CONTAINER_X + CONTAINER_BUTTON_GAP + CONTAINER_BUTTON_WIDTH);
        break;
    case ButtonColumnMiddle:
        minX = g_aspectRatioOffsetX + Scale(CONTAINER_X + CONTAINER_BUTTON_GAP);
        maxX = g_aspectRatioOffsetX + Scale(CONTAINER_X + CONTAINER_WIDTH - CONTAINER_BUTTON_GAP);
        break;
    case ButtonColumnRight:
        minX = g_aspectRatioOffsetX + Scale(CONTAINER_X + CONTAINER_WIDTH - CONTAINER_BUTTON_GAP - CONTAINER_BUTTON_WIDTH);
        maxX = g_aspectRatioOffsetX + Scale(CONTAINER_X + CONTAINER_WIDTH - CONTAINER_BUTTON_GAP);
        break;
    }
}

static void DrawSourceButton(ButtonColumn buttonColumn, float yRatio, const char *sourceText, bool sourceSet)
{
    bool buttonPressed;
    float minX, maxX;
    ComputeButtonColumnCoordinates(buttonColumn, minX, maxX);

    float minusY = (CONTAINER_BUTTON_GAP + BUTTON_HEIGHT) * yRatio;
    ImVec2 min = { minX, g_aspectRatioOffsetY + Scale(CONTAINER_Y + CONTAINER_HEIGHT - CONTAINER_BUTTON_GAP - BUTTON_HEIGHT - minusY) };
    ImVec2 max = { maxX, g_aspectRatioOffsetY + Scale(CONTAINER_Y + CONTAINER_HEIGHT - CONTAINER_BUTTON_GAP - minusY) };

    auto alphaMotion = ComputeMotionInstaller(g_appearTime, g_disappearTime, CONTAINER_INNER_TIME, CONTAINER_INNER_DURATION);
    auto lightSize = Scale(14);

    DrawButton(min, max, sourceText, true, sourceSet, buttonPressed, ((max.x - min.x) * 0.7) / g_aspectRatioScale);
}

static void DrawProgressBar(float progressRatio)
{
    auto &res = ImGui::GetIO().DisplaySize;
    auto drawList = ImGui::GetBackgroundDrawList();
    float alpha = 1.0;
    const uint32_t innerColor0 = IM_COL32(0, 65, 0, 255 * alpha);
    const uint32_t innerColor1 = IM_COL32(0, 32, 0, 255 * alpha);
    float xPadding = Scale(4);
    float yPadding = Scale(3);
    ImVec2 min = { g_aspectRatioOffsetX + Scale(CONTAINER_X) + BOTTOM_X_GAP + Scale(1), g_aspectRatioOffsetY + Scale(CONTAINER_Y + CONTAINER_HEIGHT + BOTTOM_Y_GAP)};
    ImVec2 max = { g_aspectRatioOffsetX + Scale(CONTAINER_X + CONTAINER_WIDTH - BOTTOM_X_GAP), g_aspectRatioOffsetY + Scale(CONTAINER_Y + CONTAINER_HEIGHT + BOTTOM_Y_GAP + BUTTON_HEIGHT) };

    DrawButtonContainer(min, max, 0, 0, alpha);

    drawList->AddRectFilledMultiColor
    (
        { min.x + xPadding, min.y + yPadding },
        { max.x - xPadding, max.y - yPadding },
        innerColor0,
        innerColor0,
        innerColor1,
        innerColor1
    );

    const uint32_t sliderColor0 = IM_COL32(57, 241, 0, 255 * alpha);
    const uint32_t sliderColor1 = IM_COL32(2, 106, 0, 255 * alpha);
    xPadding += Scale(1.5f);
    yPadding += Scale(1.5f);

    ImVec2 sliderMin = { min.x + xPadding, min.y + yPadding };
    ImVec2 sliderMax = { max.x - xPadding, max.y - yPadding };
    sliderMax.x = sliderMin.x + (sliderMax.x - sliderMin.x) * progressRatio;
    drawList->AddRectFilledMultiColor(sliderMin, sliderMax, sliderColor0, sliderColor0, sliderColor1, sliderColor1);
}

static bool ConvertPathSet(const nfdpathset_t *pathSet, std::list<std::filesystem::path> &filePaths)
{
    nfdpathsetsize_t pathSetCount = 0;
    if (NFD_PathSet_GetCount(pathSet, &pathSetCount) != NFD_OKAY)
    {
        return false;
    }

    for (nfdpathsetsize_t i = 0; i < pathSetCount; i++)
    {
        nfdnchar_t *pathSetPath = nullptr;
        if (NFD_PathSet_GetPathN(pathSet, i, &pathSetPath) != NFD_OKAY)
        {
            filePaths.clear();
            return false;
        }

        filePaths.emplace_back(std::filesystem::path(pathSetPath));
        NFD_PathSet_FreePathN(pathSetPath);
    }

    return true;
}

static void PickerThreadProcess()
{
    const nfdpathset_t *pathSet;
    nfdresult_t result = NFD_ERROR;
    if (g_currentPickerFolderMode)
    {
        result = NFD_PickFolderMultipleN(&pathSet, nullptr);
    }
    else
    {
        result = NFD_OpenDialogMultipleN(&pathSet, nullptr, 0, nullptr);
    }
    
    if (result == NFD_OKAY)
    {
        bool pathsConverted = ConvertPathSet(pathSet, g_currentPickerResults);
        NFD_PathSet_Free(pathSet);
    }
    else if (result == NFD_ERROR)
    {
        g_currentPickerErrorMessage = NFD_GetError();
    }

    g_currentPickerResultsReady = true;
}

static void PickerStart(bool folderMode) {
    if (g_currentPickerThread != nullptr)
    {
        g_currentPickerThread->join();
        g_currentPickerThread.reset();
    }

    g_currentPickerResults.clear();
    g_currentPickerFolderMode = folderMode;
    g_currentPickerResultsReady = false;
    g_currentPickerVisible = true;

    // Optional single thread mode for testing on systems that do not interact well with the separate thread being used for NFD.
#ifdef __APPLE__
    constexpr bool singleThreadMode = true;
#else
    constexpr bool singleThreadMode = false;
#endif
    if (singleThreadMode)
        PickerThreadProcess();
    else
        g_currentPickerThread = std::make_unique<std::thread>(PickerThreadProcess);
}

static void PickerShow(bool folderMode)
{
    if (g_pickerTutorialCleared[folderMode])
    {
        PickerStart(folderMode);
    }
    else
    {
        g_currentMessagePrompt = Localise(folderMode ? "Installer_Message_FolderPickerTutorial" : "Installer_Message_FilePickerTutorial");
        g_currentMessagePromptConfirmation = false;
        g_pickerTutorialTriggered = true;
        g_pickerTutorialFolderMode = folderMode;
    }
}

static bool ParseSourcePaths(std::list<std::filesystem::path> &paths)
{
    assert((g_currentPage == WizardPage::SelectGame) || (g_currentPage == WizardPage::SelectDLC));

    constexpr size_t failedPathLimit = 5;
    bool isFailedPathsOverLimit = false;
    std::list<std::filesystem::path> failedPaths;
    if (g_currentPage == WizardPage::SelectGame)
    {
        for (const std::filesystem::path &path : paths)
        {
            if (Installer::parseGame(path))
            {
                g_gameSourcePath = path;
            }
            else if (failedPaths.size() < failedPathLimit)
            {
                failedPaths.push_back(path);
            }
            else
            {
                isFailedPathsOverLimit = true;
            }
        }
    }
    else if(g_currentPage == WizardPage::SelectDLC)
    {
        for (const std::filesystem::path &path : paths)
        {
            DLC dlc = Installer::parseDLC(path);
            if (dlc != DLC::Unknown)
            {
                g_dlcSourcePaths[DLCIndex(dlc)] = path;
            }
            else if (failedPaths.size() < failedPathLimit)
            {
                failedPaths.push_back(path);
            }
        }
    }

    if (!failedPaths.empty())
    {
        std::stringstream stringStream;
        stringStream << Localise("Installer_Message_InvalidFilesList") << std::endl;
        for (const std::filesystem::path &path : failedPaths)
        {
            std::u8string filenameU8 = path.filename().u8string();
            stringStream << std::endl << "- " << Truncate(std::string(filenameU8.begin(), filenameU8.end()), 32, true, true);
        }

        if (isFailedPathsOverLimit)
            stringStream << std::endl << "- [...]";

        g_currentMessagePrompt = stringStream.str();
        g_currentMessagePromptConfirmation = false;
    }

    return failedPaths.empty();
}

static void DrawLanguagePicker()
{
    bool buttonPressed = false;
    if (g_currentPage == WizardPage::SelectLanguage)
    {
        float alphaMotion = ComputeMotionInstaller(g_appearTime, g_disappearTime, CONTAINER_INNER_TIME, CONTAINER_INNER_DURATION);
        float minX, maxX;
        bool buttonPressed;

        for (int i = 0; i < 6; i++)
        {
            ComputeButtonColumnCoordinates((i < 3) ? ButtonColumnLeft : ButtonColumnRight, minX, maxX);

            float minusY = (CONTAINER_BUTTON_GAP + BUTTON_HEIGHT) * (float(i % 3));
            ImVec2 min = { minX, g_aspectRatioOffsetY + Scale(CONTAINER_Y + CONTAINER_HEIGHT - CONTAINER_BUTTON_GAP - BUTTON_HEIGHT - minusY) };
            ImVec2 max = { maxX, g_aspectRatioOffsetY + Scale(CONTAINER_Y + CONTAINER_HEIGHT - CONTAINER_BUTTON_GAP - minusY) };

            auto lightSize = Scale(14);

            DrawButton(min, max, LANGUAGE_TEXT[i], false, true, buttonPressed, FLT_MAX, LANGUAGE_ENUM[i] == ELanguage::English);

            if (buttonPressed)
            {
                Config::Language = LANGUAGE_ENUM[i];
                g_commonMenu.SetTitle(Localise("Installer_Header_Installer"), false);
            }
        }
    }
}

static void DrawSourcePickers()
{
    bool buttonPressed = false;
    std::list<std::filesystem::path> paths;
    if (g_currentPage == WizardPage::SelectGame || g_currentPage == WizardPage::SelectDLC)
    {
        constexpr float ADD_BUTTON_MAX_TEXT_WIDTH = 168.0f;
        const std::string &addFilesText = Localise("Installer_Button_AddFiles");
        float squashRatio;
        ImVec2 textSize = ComputeTextSize(g_pFntRodin, addFilesText.c_str(), 20.0f, squashRatio, ADD_BUTTON_MAX_TEXT_WIDTH);
        ImVec2 min = { g_aspectRatioOffsetX + Scale(CONTAINER_X + BOTTOM_X_GAP), g_aspectRatioOffsetY + Scale(CONTAINER_Y + CONTAINER_HEIGHT + BOTTOM_Y_GAP) };
        ImVec2 max = { g_aspectRatioOffsetX + Scale(CONTAINER_X + BOTTOM_X_GAP + textSize.x * squashRatio + BUTTON_TEXT_GAP), g_aspectRatioOffsetY + Scale(CONTAINER_Y + CONTAINER_HEIGHT + BOTTOM_Y_GAP + BUTTON_HEIGHT) };
        DrawButton(min, max, addFilesText.c_str(), false, true, buttonPressed, ADD_BUTTON_MAX_TEXT_WIDTH);
        if (buttonPressed)
        {
            PickerShow(false);
        }

        min.x += Scale(BOTTOM_X_GAP + textSize.x * squashRatio + BUTTON_TEXT_GAP);

        const std::string &addFolderText = Localise("Installer_Button_AddFolder");
        textSize = ComputeTextSize(g_pFntRodin, addFolderText.c_str(), 20.0f, squashRatio, ADD_BUTTON_MAX_TEXT_WIDTH);
        max.x = min.x + Scale(textSize.x * squashRatio + BUTTON_TEXT_GAP);
        DrawButton(min, max, addFolderText.c_str(), false, true, buttonPressed, ADD_BUTTON_MAX_TEXT_WIDTH);
        if (buttonPressed)
        {
            PickerShow(true);
        }
    }
}

static void DrawSources()
{
    if (g_currentPage == WizardPage::SelectGame)
    {
        DrawSourceButton(ButtonColumnMiddle, 0, Localise("Installer_Step_Game").c_str(), !g_gameSourcePath.empty());
    }

    if (g_currentPage == WizardPage::SelectDLC)
    {
        for (int i = 0; i < 7; i++)
        {
            DrawSourceButton((i == 3) ? ButtonColumnMiddle : (i < 3) ? ButtonColumnLeft : ButtonColumnRight, 3 - float(i % 4), DLC_SOURCE_TEXT[i], !g_dlcSourcePaths[i].empty() || g_dlcInstalled[i]);
        }
    }
}

static void DrawInstallingProgress()
{
    if (g_currentPage == WizardPage::Installing)
    {
        constexpr float ProgressSpeed = 0.1f;
        float ratioTarget = g_installerProgressRatioTarget.load();
        g_installerProgressRatioCurrent += std::min(ratioTarget - g_installerProgressRatioCurrent, ProgressSpeed * ImGui::GetIO().DeltaTime);
        DrawProgressBar(g_installerProgressRatioCurrent);

        if (g_installerFinished)
        {
            g_installerThread->join();
            g_installerThread.reset();
            g_installerEndTime = ImGui::GetTime();
            SetCurrentPage(g_installerFailed ? WizardPage::InstallFailed : WizardPage::InstallSucceeded);
            g_commonMenu.SetTitle(Localise("Installer_Header_Installer"));
        }
    }
}

static void InstallerThread()
{
    if (!Installer::install(g_installerSources, g_installPath, false, g_installerJournal, std::chrono::seconds(1), [&]() {
        g_installerProgressRatioTarget = float(double(g_installerJournal.progressCounter) / double(g_installerJournal.progressTotal));

        // If user is being asked for confirmation on cancelling the installation, halt the installer from progressing further.
        g_installerHalted.wait(true);

        // If user has confirmed they wish to cancel the installation, return false to indicate the installer should fail and stop.
        return !g_installerCancelled.load();
    }))
    {
        g_installerFailed = true;
        g_installerErrorMessage = g_installerJournal.lastErrorMessage;

        // Delete all files that were copied.
        Installer::rollback(g_installerJournal);
    }

    g_installerFinished = true;
    g_installerCancelled = false;
}

static void InstallerStart()
{
    SetCurrentPage(WizardPage::Installing);
    g_installerStartTime = ImGui::GetTime();
    g_installerEndTime = DBL_MAX;
    g_installerProgressRatioCurrent = 0.0f;
    g_installerProgressRatioTarget = 0.0f;
    g_installerFailed = false;
    g_installerFinished = false;
    g_installerThread = std::make_unique<std::thread>(InstallerThread);
    g_commonMenu.SetTitle(Localise("Installer_Header_Installing"));
}

static bool InstallerParseSources(std::string &errorMessage)
{
    std::error_code spaceErrorCode;
    std::filesystem::space_info spaceInfo = std::filesystem::space(g_installPath, spaceErrorCode);
    if (!spaceErrorCode)
    {
        g_installerAvailableSize = spaceInfo.available;
    }

    Installer::Input installerInput;
    installerInput.gameSource = g_gameSourcePath;

    for (std::filesystem::path &path : g_dlcSourcePaths)
    {
        if (!path.empty())
        {
            installerInput.dlcSources.push_back(path);
        }
    }

    bool sourcesParsed = Installer::parseSources(installerInput, g_installerJournal, g_installerSources);
    errorMessage = g_installerJournal.lastErrorMessage;
    return sourcesParsed;
}

static void DrawNavigationButton()
{
    if (g_currentPage == WizardPage::Installing)
    {
        // Navigation buttons are not offered during installation at the moment.
        return;
    }

    bool nextButtonEnabled = !g_isDisappearing && (g_currentPage != WizardPage::Installing);
    if (nextButtonEnabled && g_currentPage == WizardPage::SelectGame)
    {
        nextButtonEnabled = !g_gameSourcePath.empty();
    }

    bool skipButton = false;
    if (g_currentPage == WizardPage::SelectDLC)
    {
        skipButton = std::all_of(g_dlcSourcePaths.begin(), g_dlcSourcePaths.end(), [](const std::filesystem::path &path) { return path.empty(); });
    }

    float squashRatio;
    constexpr float NAV_BUTTON_MAX_TEXT_WIDTH = 90.0f;
    std::string_view nextButtonKey = "Installer_Button_Next";
    if (skipButton)
    {
        nextButtonKey = "Installer_Button_Skip";
    }
    else if (g_currentPage == WizardPage::InstallFailed)
    {
        nextButtonKey = "Installer_Button_Retry";
    }

    const std::string &nextButtonText = Localise(nextButtonKey);
    ImVec2 nextTextSize = ComputeTextSize(g_pFntRodin, nextButtonText.c_str(), 20.0f, squashRatio, NAV_BUTTON_MAX_TEXT_WIDTH);
    ImVec2 min = { g_aspectRatioOffsetX + Scale(CONTAINER_X + CONTAINER_WIDTH - nextTextSize.x * squashRatio - BOTTOM_X_GAP - BUTTON_TEXT_GAP), g_aspectRatioOffsetY + Scale(CONTAINER_Y + CONTAINER_HEIGHT + BOTTOM_Y_GAP) };
    ImVec2 max = { g_aspectRatioOffsetX + Scale(CONTAINER_X + CONTAINER_WIDTH - BOTTOM_X_GAP), g_aspectRatioOffsetY + Scale(CONTAINER_Y + CONTAINER_HEIGHT + BOTTOM_Y_GAP + BUTTON_HEIGHT) };

    bool buttonPressed = false;
    DrawButton(min, max, nextButtonText.c_str(), false, nextButtonEnabled, buttonPressed, NAV_BUTTON_MAX_TEXT_WIDTH);

    if (buttonPressed)
    {
        if (g_currentPage == WizardPage::SelectDLC)
        {
            bool dlcInstallerMode = g_gameSourcePath.empty();
            std::string sourcesErrorMessage;
            if (!InstallerParseSources(sourcesErrorMessage))
            {
                // Some of the sources that were provided to the installer are not valid. Restart the file selection process.
                std::stringstream stringStream;
                stringStream << Localise("Installer_Message_InvalidFiles");
                if (!sourcesErrorMessage.empty()) {
                    stringStream << std::endl << std::endl << sourcesErrorMessage;
                }

                g_currentMessagePrompt = stringStream.str();
                g_currentMessagePromptConfirmation = false;
                SetCurrentPage(dlcInstallerMode ? WizardPage::SelectDLC : WizardPage::SelectGame);
            }
            else if (skipButton && dlcInstallerMode)
            {
                // Nothing was selected and the installer was in DLC mode, just close it.
                g_isDisappearing = true;
                g_disappearTime = ImGui::GetTime();
            }
            else
            {
                SetCurrentPage(WizardPage::CheckSpace);
            }
        }
        else if (g_currentPage == WizardPage::CheckSpace)
        {
            InstallerStart();
        }
        else if (g_currentPage == WizardPage::InstallSucceeded)
        {
            g_isDisappearing = true;
            g_disappearTime = ImGui::GetTime();
        }
        else if (g_currentPage == WizardPage::InstallFailed)
        {
            SetCurrentPage(g_firstPage);
        }
        else
        {
            SetCurrentPage(WizardPage(int(g_currentPage) + 1));
        }
    }
}

static void CheckCancelAction()
{
    if (!g_currentCursorBack)
    {
        return;
    }
    
    g_currentCursorBack = false;

    if (g_currentPage == WizardPage::InstallSucceeded)
    {
        // Nothing to back out on this page.
        return;
    }
    if (g_currentPage == WizardPage::Installing && g_installerCancelled)
    {
        // Installer's already been cancelled, no need for more confirmations.
        return;
    }

    Game_PlaySound("cursor2");

    if (g_currentPage == g_firstPage || g_currentPage == WizardPage::InstallFailed)
    {
        // Ask for confirmation if user wants to quit the installer.
        g_currentMessagePrompt = Localise("Installer_Message_Quit");
        g_currentMessagePromptSource = MessagePromptSource::Back;
        g_currentMessagePromptConfirmation = true;
    }
    else if (g_currentPage == WizardPage::Installing)
    {
        // Ask for confirmation if the user wants to cancel the installation.
        g_currentMessagePrompt = Localise("Installer_Message_Cancel");
        g_currentMessagePromptSource = MessagePromptSource::Back;
        g_currentMessagePromptConfirmation = true;

        // Indicate to the installer that all progress should stop until the user confirms if they wish to cancel.
        g_installerHalted = true;
    }
    else if (int(g_currentPage) > 0)
    {
        // Just go back to the previous page.
        SetCurrentPage(WizardPage(int(g_currentPage) - 1));
    }
}

static void DrawMessagePrompt()
{
    if (g_currentMessagePrompt.empty())
    {
        return;
    }

    bool messageWindowReturned = false;
    if (g_currentMessagePromptConfirmation)
    {
        std::array<std::string, 2> YesNoButtons = { Localise("Common_Yes"), Localise("Common_No") };
        messageWindowReturned = MessageWindow::Open(g_currentMessagePrompt, &g_currentMessageResult, YesNoButtons, 1);
    }
    else
    {
        messageWindowReturned = MessageWindow::Open(g_currentMessagePrompt, &g_currentMessageResult);
    }

    if (messageWindowReturned)
    {
        if (g_currentMessagePromptConfirmation && (g_currentMessageResult == 0))
        {
            if (g_currentMessagePromptSource == MessagePromptSource::Back)
            {
                if (g_currentPage == WizardPage::Installing)
                {
                    // If user confirms they wish to cancel the installation, notify the installation thread it must finish as soon as possible.
                    g_installerCancelled = true;
                }
                else
                {
                    // In all cases, proceed to just quit the application.
                    g_isQuitting = true;
                    g_isDisappearing = true;
                    g_disappearTime = ImGui::GetTime();
                }
            }
            else if (g_currentPage == WizardPage::SelectDLC)
            {
                // If user confirms the message prompt that they wish to skip installing the DLC, proceed to the next step.
                SetCurrentPage(WizardPage::CheckSpace);
            }
        }

        if (g_currentMessagePromptSource == MessagePromptSource::Back)
        {
            // Regardless of the confirmation, the installation thread must be resumed.
            g_installerHalted = false;
            g_installerHalted.notify_all();
        }

        g_currentMessagePrompt.clear();
        g_currentMessagePromptSource = MessagePromptSource::Unknown;
        g_currentMessageResult = -1;
    }
}

static void PickerDrawForeground()
{
    if (g_currentPickerVisible)
    {
        auto drawList = ImGui::GetBackgroundDrawList();
        drawList->AddRectFilled({ 0.0f, 0.0f }, ImGui::GetIO().DisplaySize, IM_COL32(0, 0, 0, 190));
    }
}

static void PickerCheckTutorial()
{
    if (!g_pickerTutorialTriggered || !g_currentMessagePrompt.empty())
    {
        return;
    }

    PickerStart(g_pickerTutorialFolderMode);
    g_pickerTutorialTriggered = false;
}

static void PickerCheckResults()
{
    if (!g_currentPickerResultsReady)
    {
        return;
    }

    if (!g_currentPickerErrorMessage.empty())
    {
        g_currentMessagePrompt = g_currentPickerErrorMessage;
        g_currentMessagePromptConfirmation = false;
        g_currentPickerErrorMessage.clear();
    }

    if (!g_currentPickerResults.empty() && ParseSourcePaths(g_currentPickerResults))
    {
        g_pickerTutorialCleared[g_pickerTutorialFolderMode] = true;
    }

    g_currentPickerResultsReady = false;
    g_currentPickerVisible = false;
}

static bool g_fadingOutMusic;

static void ProcessMusic()
{
    if (g_isDisappearing)
    {
        if (!g_fadingOutMusic)
        {
            EmbeddedPlayer::FadeOutMusic();
            g_fadingOutMusic = true;
        }
    }
    else
    {
        EmbeddedPlayer::PlayMusic();
    }
}

void InstallerWizard::Init()
{
    auto &io = ImGui::GetIO();

    g_commonMenu = CommonMenu(Localise("Installer_Header_Installer"), "", true);
    g_installTextures[0] = LOAD_ZSTD_TEXTURE(g_install_001);
    g_installTextures[1] = LOAD_ZSTD_TEXTURE(g_install_002);
    g_installTextures[2] = LOAD_ZSTD_TEXTURE(g_install_003);
    g_installTextures[3] = LOAD_ZSTD_TEXTURE(g_install_004);
    g_installTextures[4] = LOAD_ZSTD_TEXTURE(g_install_005);
    g_installTextures[5] = LOAD_ZSTD_TEXTURE(g_install_006);
    g_installTextures[6] = LOAD_ZSTD_TEXTURE(g_install_007);
    g_installTextures[7] = LOAD_ZSTD_TEXTURE(g_install_008);
    g_upLibertyDev = LOAD_ZSTD_TEXTURE(g_libertyrecomp);

    for (int i = 0; i < g_credits.size(); i++)
    {
        g_creditsStr += g_credits[i];
        g_creditsStr += "  ";
    }
}

void InstallerWizard::Draw()
{
    if (!s_isVisible)
        return;

    ResetCursorRects();
    DrawBackground();
    DrawArrows();
    DrawLeftImage();
    g_commonMenu.Draw();
    DrawDescriptionContainer();
    DrawLanguagePicker();
    DrawSourcePickers();
    DrawSources();
    DrawInstallingProgress();
    DrawNavigationButton();
    CheckCancelAction();
    DrawMessagePrompt();
    PickerDrawForeground();
    PickerCheckTutorial();
    PickerCheckResults();

    if (g_isDisappearing)
    {
        double disappearDuration = ALL_ANIMATIONS_FULL_DURATION / 60.0;
        if (g_isQuitting)
        {
            // Add some extra waiting time when quitting the application altogether.
            disappearDuration += QUITTING_EXTRA_DURATION / 60.0;
        }

        if (ImGui::GetTime() > (g_disappearTime + disappearDuration))
        {
            s_isVisible = false;
        }
    }
}

void InstallerWizard::Shutdown()
{
    // Wait for and erase the threads.
    if (g_installerThread != nullptr)
    {
        g_installerThread->join();
        g_installerThread.reset();
    }

    if (g_currentPickerThread != nullptr)
    {
        g_currentPickerThread->join();
        g_currentPickerThread.reset();
    }

    // Erase the sources.
    g_installerSources.game.reset();
    g_installerSources.dlc.clear();
    
    // Make sure the GPU is not currently active before deleting these textures.
    Video::WaitForGPU();

    // Erase the textures.
    for (auto &texture : g_installTextures)
    {
        texture.reset();
    }
}

bool InstallerWizard::Run(std::filesystem::path installPath, bool skipGame)
{
    g_installPath = installPath;

    EmbeddedPlayer::Init();
    NFD_Init();

    // Guarantee one controller is initialized. We'll rely on SDL's event loop to get the controller events.
    XAMINPUT_STATE inputState;
    hid::GetState(0, &inputState);

    if (skipGame)
    {
        for (int i = 0; i < int(DLC::Count); i++)
        {
            g_dlcInstalled[i] = Installer::checkDLCInstall(g_installPath, DLC(i + 1));
        }

        g_firstPage = WizardPage::SelectDLC;
    }

    SetCurrentPage(g_firstPage);

    GameWindow::SetFullscreenCursorVisibility(true);
    s_isVisible = true;

    while (s_isVisible)
    {
        Video::WaitOnSwapChain();
        ProcessMusic();
        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        GameWindow::Update();
        Video::Present();
    }

    GameWindow::SetFullscreenCursorVisibility(false);
    NFD_Quit();

    InstallerWizard::Shutdown();
    EmbeddedPlayer::Shutdown();

    return !g_isQuitting;
}
