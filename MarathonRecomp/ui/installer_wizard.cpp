#include "installer_wizard.h"

#include <apu/embedded_player.h>
#include <gpu/video.h>
#include <gpu/imgui/imgui_snapshot.h>
#include <hid/hid.h>
#include <install/installer.h>
#include <locale/locale.h>
#include <patches/aspect_ratio_patches.h>
#include <ui/button_window.h>
#include <ui/common_menu.h>
#include <ui/fader.h>
#include <ui/game_window.h>
#include <ui/imgui_utils.h>
#include <ui/message_window.h>
#include <decompressor.h>
#include <exports.h>
#include <nfd.h>
#include <sdl_listener.h>

#include <res/images/installer/install_001.dds.h>
#include <res/images/installer/install_002.dds.h>
#include <res/images/installer/install_003.dds.h>
#include <res/images/installer/install_004.dds.h>
#include <res/images/installer/install_005.dds.h>
#include <res/images/installer/install_006.dds.h>
#include <res/images/installer/install_007.dds.h>
#include <res/images/installer/install_008.dds.h>
#include <res/images/common/sonicnext-dev.dds.h>
#include <res/credits.h>

static constexpr float COMMON_MENU_INTRO_TIME = 10.0f;
static constexpr float COMMON_FADE_TIME = 25.0f;

static constexpr float NAV_BUTTON_OFFSET_Y = 39.0f;
static constexpr float NAV_BUTTON_MARGIN = 48.0f;

static CommonMenu g_commonMenu{};

static std::array<std::unique_ptr<GuestTexture>, 8> g_installTextures{};
static std::unique_ptr<GuestTexture> g_upTexSonicNextDev{};

static double g_chevronTime{};
static double g_cursorArrowsTime{};
static double g_appearTime{};

static double g_alphaTime{};
static double g_alphaMotion{};

static bool g_isIntroAnim{ true };
static bool g_isDisappearing{};
static bool g_isQuitting{};

static std::filesystem::path g_installPath{};
static std::filesystem::path g_gameSourcePath{};
static std::array<std::filesystem::path, int(DLC::Count)> g_dlcSourcePaths{};
static std::array<bool, int(DLC::Count)> g_dlcInstalled{};

static Journal g_installerJournal{};
static Installer::Sources g_installerSources{};
static uint64_t g_installerAvailableSize{};
static std::unique_ptr<std::thread> g_installerThread{};
static double g_installerStartTime{};
static double g_installerEndTime{ DBL_MAX };
static float g_installerProgressRatioCurrent{};
static std::atomic<float> g_installerProgressRatioTarget{};
static std::atomic<bool> g_installerFinished{};
static std::atomic<bool> g_installerHalted{};
static std::atomic<bool> g_installerCancelled{};
static bool g_installerFailed{};
static std::string g_installerErrorMessage{};

static std::array<ImVec2, 8> g_installTexturePositions =
{
    ImVec2(110.0f, 90.0f),  // Sonic
    ImVec2(-20.0f, 90.0f),  // Shadow
    ImVec2(110.0f, 10.0f),  // Silver
    ImVec2(25.0f, 80.0f),   // Tails
    ImVec2(100.0f, 90.0f),  // Rouge
    ImVec2(110.0f, 90.0f),  // Amy
    ImVec2(180.0f, 140.0f), // Elise
    ImVec2(0.0f, 140.0f)    // Eggman
};

static const int g_installTextureIndices[] =
{
    0, // SelectLanguage
    1, // Introduction
    0, // SelectGame -- this page doesn't display a character.
    0, // SelectDLC --- this page doesn't display a character.
    2, // CheckSpace
    3, // Installing
    6, // InstallSucceeded -- force Elise.
    7  // InstallFailed ----- force Eggman.
};

const char* g_languageText[] =
{
    "English",  // English
    "日本語",    // Japanese
    "Deutsch",  // German
    "Français", // French
    "Español",  // Spanish
    "Italiano"  // Italian
};

const ELanguage g_languageEnum[] =
{
    ELanguage::English,
    ELanguage::Japanese,
    ELanguage::German,
    ELanguage::French,
    ELanguage::Spanish,
    ELanguage::Italian
};

const char* g_dlcText[] =
{
    "Sonic Boss Attack",
    "Shadow Boss Attack",
    "Silver Boss Attack",
    "Team Attack Amigo",
    "Sonic/Very Hard",
    "Shadow/Very Hard",
    "Silver/Very Hard",
};

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

static WizardPage g_firstPage{};
static WizardPage g_currentPage = g_firstPage;

static std::string g_currentMessagePrompt{};
static MessagePromptSource g_currentMessagePromptSource{};
static bool g_currentMessagePromptConfirmation{};
static int g_currentMessageResult{ -1 };

static std::list<std::filesystem::path> g_currentPickerResults{};
static std::atomic<bool> g_currentPickerResultsReady{};
static std::string g_currentPickerErrorMessage{};
static std::unique_ptr<std::thread> g_currentPickerThread{};

static bool g_pickerTutorialCleared[2]{};
static bool g_pickerTutorialTriggered{};
static bool g_pickerTutorialFolderMode{};
static bool g_currentPickerVisible{};
static bool g_currentPickerFolderMode{};

static int g_currentCursorIndex{ -1 };
static int g_currentCursorDefault{};
static bool g_currentCursorAccepted{};
static bool g_currentCursorBack{};
static std::vector<std::pair<ImVec2, ImVec2>> g_currentCursorRects{};

static std::string g_creditsStr{};

class SDLEventListenerForInstaller : public SDLEventListener
{
    ImVec2 m_joypadAxis = {};

public:
    bool OnSDLEvent(SDL_Event* event) override
    {
        if (!InstallerWizard::s_isVisible || g_isDisappearing)
            return false;

        auto noModals = g_currentMessagePrompt.empty() && !g_currentPickerVisible;

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

        constexpr auto axisValueRange = 32767.0f;
        constexpr auto axisTapRange = 0.5f;
        auto newCursorIndex = -1;
        auto tapDirection = ImVec2();

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
                    auto newAxisValue = event->caxis.value / axisValueRange;
                    auto sameDirection = (newAxisValue * m_joypadAxis[event->caxis.axis]) > 0.0f;
                    auto wasInRange = abs(m_joypadAxis[event->caxis.axis]) > axisTapRange;
                    auto isInRange = abs(newAxisValue) > axisTapRange;

                    if (sameDirection && !wasInRange && isInRange)
                        tapDirection[event->caxis.axis] = newAxisValue;

                    m_joypadAxis[event->caxis.axis] = newAxisValue;
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
                auto& currentRect = g_currentCursorRects[g_currentCursorIndex];

                auto currentPoint = ImVec2
                (
                    (currentRect.first.x + currentRect.second.x) / 2.0f + tapDirection.x * (currentRect.second.x - currentRect.first.x) / 2.0f,
                    (currentRect.first.y + currentRect.second.y) / 2.0f + tapDirection.y * (currentRect.second.y - currentRect.first.y) / 2.0f
                );

                auto closestDistance = FLT_MAX;

                for (size_t i = 0; i < g_currentCursorRects.size(); i++)
                {
                    if (g_currentCursorIndex == i)
                        continue;

                    auto& targetRect = g_currentCursorRects[i];

                    auto targetPoint = ImVec2
                    (
                        (targetRect.first.x + targetRect.second.x) / 2.0f + tapDirection.x * (targetRect.first.x - targetRect.second.x) / 2.0f,
                        (targetRect.first.y + targetRect.second.y) / 2.0f + tapDirection.y * (targetRect.first.y - targetRect.second.y) / 2.0f
                    );

                    auto delta = ImVec2(targetPoint.x - currentPoint.x, targetPoint.y - currentPoint.y);
                    auto projectedDistance = delta.x * tapDirection.x + delta.y * tapDirection.y;
                    auto manhattanDistance = abs(delta.x) + abs(delta.y);

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
            {
                Game_PlaySound("move");
                g_cursorArrowsTime = ImGui::GetTime();
            }

            g_currentCursorIndex = newCursorIndex;
        }

        return false;
    }
}
g_sdlEventListenerForInstaller;

static void LeaveInstallerWizard(bool isQuitting = false)
{
    g_isDisappearing = true;

    Fader::FadeOut
    (
        1.0f,

        [=]()
        {
            g_isQuitting = isQuitting;
            InstallerWizard::s_isVisible = false;
        }
    );
}

static int DLCIndex(DLC dlc)
{
    assert(dlc != DLC::Unknown);

    return (int)(dlc) - 1;
}

static void SetCurrentPage(WizardPage page)
{
    g_currentPage = page;
    g_cursorArrowsTime = ImGui::GetTime();

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

static bool PushCursorRect(ImVec2 min, ImVec2 max, bool& isPressed, bool isDefault = false)
{
    auto currentIndex = int(g_currentCursorRects.size());

    g_currentCursorRects.emplace_back(min, max);

    if (isDefault)
        g_currentCursorDefault = currentIndex;

    if (g_currentCursorIndex == currentIndex)
    {
        if (g_currentCursorAccepted)
        {
            isPressed = true;
            g_currentCursorAccepted = false;
        }

        return true;
    }

    return false;
}

static void DrawProgressBar(ImVec2 originMin, ImVec2 originMax, float progress)
{
    auto& res = ImGui::GetIO().DisplaySize;
    auto  drawList = ImGui::GetBackgroundDrawList();

    auto pos = ImVec2(originMin.x - Scale(8, true), originMax.y - Scale(NAV_BUTTON_OFFSET_Y, true));
    auto edgeUVs = PIXELS_TO_UV_COORDS(256, 256, 1, 0, 51, 45);
    auto stretchUVs = PIXELS_TO_UV_COORDS(256, 256, 51, 0, 51, 45);
    auto edgeWidth = Scale(50, true);
    auto edgeHeight = Scale(45, true);
    auto colour = IM_COL32(255, 255, 255, 255 * g_alphaMotion);

    auto leftEdgeMin = pos;
    auto leftEdgeMax = ImVec2(leftEdgeMin.x + edgeWidth, leftEdgeMin.y + edgeHeight);
    auto stretchMin = ImVec2(leftEdgeMax.x, leftEdgeMin.y);
    auto stretchMax = ImVec2(stretchMin.x + Scale(400, true), leftEdgeMax.y);
    auto rightEdgeMin = ImVec2(stretchMax.x, stretchMin.y);
    auto rightEdgeMax = ImVec2(rightEdgeMin.x + edgeWidth, rightEdgeMin.y + edgeHeight);

    drawList->AddImage(g_upTexMainMenu8.get(), leftEdgeMin, leftEdgeMax, GET_UV_COORDS(edgeUVs), colour);
    drawList->AddImage(g_upTexMainMenu8.get(), stretchMin, stretchMax, GET_UV_COORDS(stretchUVs), colour);
    AddImageFlipped(g_upTexMainMenu8.get(), rightEdgeMin, rightEdgeMax, GET_UV_COORDS(edgeUVs), colour, true);

    auto gaugeOffsetX = Scale(41, true);
    auto gaugeHeight = Scale(11, true);
    auto gaugeMin = ImVec2(leftEdgeMin.x + gaugeOffsetX, (rightEdgeMin.y + edgeHeight / 2) - (gaugeHeight / 2) + Scale(1, true));
    auto gaugeMax = ImVec2(rightEdgeMax.x - gaugeOffsetX, gaugeMin.y + gaugeHeight / 2);

    drawList->AddRectFilled(gaugeMin, gaugeMax, IM_COL32(0, 0, 0, 255 * g_alphaMotion), Scale(10, true));
    drawList->AddRectFilled(gaugeMin, { gaugeMin.x + (gaugeMax.x - gaugeMin.x) * progress, gaugeMax.y }, IM_COL32(112, 250, 255, 255 * g_alphaMotion), Scale(10, true));
}

static bool ConvertPathSet(const nfdpathset_t *pathSet, std::list<std::filesystem::path> &filePaths)
{
    nfdpathsetsize_t pathSetCount = 0;

    if (NFD_PathSet_GetCount(pathSet, &pathSetCount) != NFD_OKAY)
        return false;

    for (nfdpathsetsize_t i = 0; i < pathSetCount; i++)
    {
        nfdnchar_t* pathSetPath = nullptr;

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
    auto result = NFD_ERROR;

    const nfdpathset_t* pathSet;

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
        auto pathsConverted = ConvertPathSet(pathSet, g_currentPickerResults);

        NFD_PathSet_Free(pathSet);
    }
    else if (result == NFD_ERROR)
    {
        g_currentPickerErrorMessage = NFD_GetError();
    }

    g_currentPickerResultsReady = true;
}

static void PickerStart(bool folderMode)
{
    if (g_currentPickerThread != nullptr)
    {
        g_currentPickerThread->join();
        g_currentPickerThread.reset();
    }

    g_currentPickerResults.clear();
    g_currentPickerFolderMode = folderMode;
    g_currentPickerResultsReady = false;
    g_currentPickerVisible = true;

    // Optional single thread mode for testing on systems
    // that do not interact well with the separate thread
    // being used for NFD.
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

    constexpr auto failedPathLimit = 5;
    auto isFailedPathsOverLimit = false;

    std::list<std::filesystem::path> failedPaths{};

    if (g_currentPage == WizardPage::SelectGame)
    {
        for (const auto& path : paths)
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
    else if (g_currentPage == WizardPage::SelectDLC)
    {
        for (const auto& path : paths)
        {
            auto dlc = Installer::parseDLC(path);

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
        std::stringstream stringStream{};

        stringStream << Localise("Installer_Message_InvalidFilesList") << std::endl;

        for (const auto& path : failedPaths)
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

static void InstallerThread()
{
    auto result = Installer::install(g_installerSources, g_installPath, false, g_installerJournal, std::chrono::seconds(1),
    //
        [&]()
        {
            g_installerProgressRatioTarget = float(double(g_installerJournal.progressCounter) / double(g_installerJournal.progressTotal));

            // If user is being asked for confirmation on cancelling
            // the installation, halt the installer from progressing further.
            g_installerHalted.wait(true);

            // If user has confirmed they wish to cancel the
            // installation, return false to indicate the installer
            // should fail and stop.
            return !g_installerCancelled.load();
        }
    );

    if (!result)
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
    std::error_code spaceInfoErrorCode;
    auto spaceInfo = std::filesystem::space(g_installPath, spaceInfoErrorCode);

    if (!spaceInfoErrorCode)
        g_installerAvailableSize = spaceInfo.available;

    Installer::Input installerInput{};
    installerInput.gameSource = g_gameSourcePath;

    for (auto& path : g_dlcSourcePaths)
    {
        if (path.empty())
            continue;

        installerInput.dlcSources.push_back(path);
    }

    auto sourcesParsed = Installer::parseSources(installerInput, g_installerJournal, g_installerSources);

    errorMessage = g_installerJournal.lastErrorMessage;

    return sourcesParsed;
}

static void CheckCancelAction()
{
    if (!g_currentCursorBack)
        return;
    
    g_currentCursorBack = false;

    if (g_currentPage == WizardPage::InstallSucceeded)
    {
        // Nothing to back out on this page.
        return;
    }
    if (g_currentPage == WizardPage::Installing && g_installerCancelled)
    {
        // Installer's already been cancelled,
        // no need for more confirmations.
        return;
    }

    Game_PlaySound("window_close");

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

        // Indicate to the installer that all progress should
        // stop until the user confirms if they wish to cancel.
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
    static auto s_messageWindowOpened = false;

    if (g_currentMessagePrompt.empty())
        return;

    auto messageWindowReturned = false;

    if (!s_messageWindowOpened)
    {
        // Update alpha time to fade out.
        g_alphaTime = ImGui::GetTime();
        s_messageWindowOpened = true;
    }

    if (g_currentMessagePromptConfirmation)
    {
        std::array<std::string, 2> buttons = { Localise("Common_Yes"), Localise("Common_No") };

        messageWindowReturned = MessageWindow::Open(g_currentMessagePrompt, &g_currentMessageResult, buttons, 1);
    }
    else
    {
        messageWindowReturned = MessageWindow::Open(g_currentMessagePrompt, &g_currentMessageResult);
    }

    if (messageWindowReturned)
    {
        if (g_currentMessagePromptConfirmation && !g_currentMessageResult)
        {
            if (g_currentMessagePromptSource == MessagePromptSource::Back)
            {
                if (g_currentPage == WizardPage::Installing)
                {
                    // If user confirms they wish to cancel the
                    // installation, notify the installer thread
                    // it must finish as soon as possible.
                    g_installerCancelled = true;
                }
                else
                {
                    // In all cases, proceed to just quit the application.
                    LeaveInstallerWizard(true);
                }
            }
            else if (g_currentPage == WizardPage::SelectDLC)
            {
                // If user confirms the message prompt that
                // they wish to skip installing the DLC, proceed
                // to the next step.
                SetCurrentPage(WizardPage::CheckSpace);
            }
        }

        if (g_currentMessagePromptSource == MessagePromptSource::Back)
        {
            // Regardless of the confirmation, the
            // installation thread must be resumed.
            g_installerHalted = false;
            g_installerHalted.notify_all();
        }

        g_currentMessagePrompt.clear();
        g_currentMessagePromptSource = MessagePromptSource::Unknown;
        g_currentMessageResult = -1;

        // Update alpha time to fade in.
        g_alphaTime = ImGui::GetTime();

        s_messageWindowOpened = false;
    }
}

static void PickerDrawForeground()
{
    if (!g_currentPickerVisible)
        return;

    auto drawList = ImGui::GetBackgroundDrawList();

    drawList->AddRectFilled({ 0.0f, 0.0f }, ImGui::GetIO().DisplaySize, IM_COL32(0, 0, 0, 190));
}

static void PickerCheckTutorial()
{
    if (!g_pickerTutorialTriggered || !g_currentMessagePrompt.empty())
        return;

    PickerStart(g_pickerTutorialFolderMode);

    g_pickerTutorialTriggered = false;
}

static void PickerCheckResults()
{
    if (!g_currentPickerResultsReady)
        return;

    if (!g_currentPickerErrorMessage.empty())
    {
        g_currentMessagePrompt = g_currentPickerErrorMessage;
        g_currentMessagePromptConfirmation = false;
        g_currentPickerErrorMessage.clear();
    }

    if (!g_currentPickerResults.empty() && ParseSourcePaths(g_currentPickerResults))
        g_pickerTutorialCleared[g_pickerTutorialFolderMode] = true;

    g_currentPickerResultsReady = false;
    g_currentPickerVisible = false;
}

static void DrawLeftImage()
{
    // Don't display character renders at game and DLC
    // select pages, as we draw the sources on the left side.
    if (g_currentPage == WizardPage::SelectGame || g_currentPage == WizardPage::SelectDLC)
        return;

    auto& res = ImGui::GetIO().DisplaySize;
    auto  drawList = ImGui::GetBackgroundDrawList();
    auto  installTextureIndex = g_installTextureIndices[int(g_currentPage)];

    // Cycle through the available images while time passes during installation.
    if (g_currentPage == WizardPage::Installing)
    {
        constexpr auto installSpeed = 1.0 / 15.0;
        auto installTime = (ImGui::GetTime() - g_installerStartTime) * installSpeed;

        installTextureIndex += int(installTime);
    }

    auto pTexture = g_installTextures[installTextureIndex % g_installTextures.size()].get();

    constexpr float imageSize = 1000.0f;

    auto pos = g_installTexturePositions[installTextureIndex % g_installTextures.size()];
    auto min = ImVec2(g_aspectRatioOffsetX + Scale(pos.x, true), g_aspectRatioOffsetY + Scale(pos.y, true));
    auto max = ImVec2(min.x + Scale(imageSize, true), min.y + Scale(imageSize, true));

    drawList->AddImage(pTexture, min, max, { 0, 0 }, { 1, 1 }, IM_COL32(255, 255, 255, std::lround(140.0 * g_alphaMotion)));
}

static std::string& GetWizardText(WizardPage page)
{
    switch (page)
    {
        case WizardPage::SelectLanguage:   return Localise("Installer_Page_SelectLanguage");
        case WizardPage::Introduction:     return Localise("Installer_Page_Introduction");
        case WizardPage::SelectGame:       return Localise("Installer_Page_SelectGame");
        case WizardPage::SelectDLC:        return Localise("Installer_Page_SelectDLC");
        case WizardPage::CheckSpace:       return Localise("Installer_Page_CheckSpace");
        case WizardPage::Installing:       return Localise("Installer_Page_Installing");
        case WizardPage::InstallSucceeded: return Localise("Installer_Page_InstallSucceeded");
        case WizardPage::InstallFailed:    return Localise("Installer_Page_InstallFailed");
    }

    return g_localeMissing;
}

static bool DrawButton(ImVec2 pos, const char* text, bool& isHovered, bool isEnabled, bool isDefault = false)
{
    auto& res = ImGui::GetIO().DisplaySize;
    auto  drawList = ImGui::GetBackgroundDrawList();

    auto edgeUVs = PIXELS_TO_UV_COORDS(256, 256, 1, 0, 51, 45);
    auto stretchUVs = PIXELS_TO_UV_COORDS(256, 256, 51, 0, 51, 45);
    auto width = Scale(50, true);
    auto height = Scale(45, true);
    auto colourMotion = isHovered && isEnabled ? 255 : 0;
    auto colour = IM_COL32(colourMotion, colourMotion, colourMotion, 245 * g_alphaMotion);
    auto fadeColour = IM_COL32(colourMotion, colourMotion, colourMotion, 45 * g_alphaMotion);

    auto edgeMin = ImVec2(pos.x - Scale(8, true), pos.y);
    auto edgeMax = ImVec2(edgeMin.x + width, edgeMin.y + height);
    auto stretchMin = ImVec2(edgeMax.x, edgeMin.y);
    auto stretchMax = ImVec2(res.x, edgeMax.y);
    auto fadeMin = ImVec2(stretchMax.x - ((stretchMax.x - edgeMin.x) / 2), stretchMax.y);
    auto fadeMax = stretchMax;

    SetHorizontalGradient(fadeMin, fadeMax, colour, fadeColour);
    drawList->AddImage(g_upTexMainMenu8.get(), edgeMin, edgeMax, GET_UV_COORDS(edgeUVs), colour);
    drawList->AddImage(g_upTexMainMenu8.get(), stretchMin, stretchMax, GET_UV_COORDS(stretchUVs), colour);
    ResetGradient();

    auto textPos = ImVec2(edgeMin.x + Scale(51, true), edgeMin.y + Scale(8, true));

    auto textColour = isEnabled
        ? IM_COL32(255, 255, 255, 255 * g_alphaMotion)
        : IM_COL32(137, 137, 137, 255 * g_alphaMotion);

    drawList->AddText(g_pFntRodin, g_fntRodinSize, textPos, textColour, text);

    if (isHovered && g_alphaMotion >= 1.0f)
        DrawArrowCursor({ edgeMin.x + Scale(8, true), edgeMin.y + Scale(9, true) }, g_cursorArrowsTime, true, false);

    auto isPressed = false;

    isHovered = PushCursorRect(edgeMin, fadeMax, isPressed, isDefault);

    if (isPressed)
        Game_PlaySound(isEnabled ? "main_deside" : "cannot_deside");

    return isEnabled && isPressed;
}

static void DrawSource(ImVec2 pos, const char* text, bool isEnabled)
{
    auto& res = ImGui::GetIO().DisplaySize;
    auto  drawList = ImGui::GetBackgroundDrawList();

    auto selectedUVs = PIXELS_TO_UV_COORDS(1024, 1024, 443, 524, 560, 47);
    auto unselectedUVs = PIXELS_TO_UV_COORDS(1024, 1024, 443, 579, 560, 47);
    auto categoryWidth = Scale(560, true);
    auto categoryHeight = Scale(47, true);
    auto colour = IM_COL32(255, 255, 255, 255 * g_alphaMotion);

    ImVec2 categoryMin = { pos.x + Scale(42, true), pos.y + Scale(147, true) };
    ImVec2 categoryMax = { categoryMin.x + categoryWidth, categoryMin.y + categoryHeight };

    SetHorizontalGradient(categoryMin, categoryMax, colour, IM_COL32_WHITE_TRANS);
    drawList->AddImage(g_upTexMainMenu7.get(), categoryMin, categoryMax, GET_UV_COORDS(isEnabled ? selectedUVs : unselectedUVs));
    ResetGradient();

    drawList->AddText(g_pFntRodin, g_fntRodinSize, { categoryMin.x + Scale(129, true), categoryMin.y + Scale(6, true) }, colour, text);

    if (isEnabled)
    {
        auto cursorOffsetX = Scale(80, true);
        auto cursorOffsetY = Scale(8, true);

        DrawArrowCursor({ categoryMin.x + cursorOffsetX, categoryMin.y + cursorOffsetY }, 0, false, true);
    }
}

static ImVec2 GetNavButtonPosition(ImVec2 originMin, ImVec2 originMax, int index)
{
    return { originMin.x, originMax.y - Scale(NAV_BUTTON_OFFSET_Y + (NAV_BUTTON_MARGIN * index), true) };
}

static void DrawSelectLanguagePage(ImVec2 originMin, ImVec2 originMax)
{
    auto languageBtnIdx = 5;

    for (auto& language : g_languageEnum)
    {
        auto isHovered = Config::Language == language;
        auto isPressed = DrawButton(GetNavButtonPosition(originMin, originMax, languageBtnIdx), g_languageText[int(language) - 1], isHovered, true);

        if (isHovered)
        {
            Config::Language = language;
            g_commonMenu.SetTitle(Localise("Installer_Header_Installer"), false);
        }

        if (isPressed)
            SetCurrentPage(WizardPage::Introduction);

        languageBtnIdx--;
    }
}

static void DrawSourcePickerPage(ImVec2 min, ImVec2 max, ImVec2 originMin, ImVec2 originMax)
{
    if (g_currentPage == WizardPage::SelectGame)
    {
        DrawSource(min, Localise("Installer_Step_Game").c_str(), !g_gameSourcePath.empty());
    }
    else if (g_currentPage == WizardPage::SelectDLC)
    {
        auto offsetY = 0.0f;

        for (int i = 0; i < 7; i++)
        {
            DrawSource({ min.x, min.y + offsetY }, g_dlcText[i], !g_dlcSourcePaths[i].empty() || g_dlcInstalled[i]);
            offsetY += Scale(NAV_BUTTON_MARGIN, true);
        }
    }

    static auto s_isAddFilesHovered = false;
    static auto s_isAddFolderHovered = false;

    auto isAddFilesPressed = DrawButton(GetNavButtonPosition(originMin, originMax, 2), Localise("Installer_Button_AddFiles").c_str(), s_isAddFilesHovered, true);
    auto isAddFolderPressed = DrawButton(GetNavButtonPosition(originMin, originMax, 1), Localise("Installer_Button_AddFolder").c_str(), s_isAddFolderHovered, true);

    if (isAddFilesPressed)
        PickerShow(false);

    if (isAddFolderPressed)
        PickerShow(true);
}

static void DrawInstallingPage(ImVec2 originMin, ImVec2 originMax)
{
    constexpr auto progressSpeed = 0.1f;
    auto ratioTarget = g_installerProgressRatioTarget.load();

    g_installerProgressRatioCurrent += std::min(ratioTarget - g_installerProgressRatioCurrent, progressSpeed * ImGui::GetIO().DeltaTime);

    DrawProgressBar(originMin, originMax, g_installerProgressRatioCurrent);

    if (g_installerFinished)
    {
        g_installerThread->join();
        g_installerThread.reset();

        g_installerEndTime = ImGui::GetTime();

        SetCurrentPage(g_installerFailed ? WizardPage::InstallFailed : WizardPage::InstallSucceeded);

        g_commonMenu.SetTitle(Localise("Installer_Header_Installer"));
    }
}

static void DrawInstallSucceededPage(ImVec2 originMin, ImVec2 originMax, ImVec2 descriptionTextSize)
{
    auto drawList = ImGui::GetBackgroundDrawList();

    auto marqueeTextSize = g_pFntRodin->CalcTextSizeA(g_fntRodinSize, FLT_MAX, 0, g_creditsStr.c_str());
    auto marqueeTextMarginX = Scale(5, true);
    auto marqueeTextMarginY = Scale(65, true);
    auto marqueeTextPos = ImVec2(originMax.x, originMax.y - marqueeTextSize.y - marqueeTextMarginY);
    auto marqueeTextMin = ImVec2(originMin.x, marqueeTextPos.y);
    auto marqueeTextMax = ImVec2(originMax.x, originMax.y);

    // NOTE (Hyper): shifting the first four rows of pixels out of view
    // fixes a dark line appearing at the top of the logo, along with a
    // couple chunks being taken away towards the upper right.
    //
    // This issue seems to occur a lot with various other textures,
    // needing at least one or two pixels shifted right or down to
    // prevent this error. Weird.
    auto imageUVs = PIXELS_TO_UV_COORDS(5243, 450, 0, 4, 5243, 446);

    auto imageWidth = Scale(524, true);
    auto imageHeight = Scale(45, true);
    auto imageRegionMin = ImVec2(originMin.x, originMin.y + descriptionTextSize.y);
    auto imageRegionMax = ImVec2(originMax.x, originMax.y - (marqueeTextMax.y - marqueeTextMin.y));

    auto imageMin = ImVec2
    (
        /* X */ imageRegionMin.x + ((imageRegionMax.x - imageRegionMin.x) / 2) - (imageWidth / 2),
        /* Y */ imageRegionMin.y + ((imageRegionMax.y - imageRegionMin.y) / 2) - (imageHeight / 2)
    );

    auto imageMax = ImVec2(imageMin.x + imageWidth, imageMin.y + imageHeight);

    drawList->AddImage(g_upTexSonicNextDev.get(), imageMin, imageMax, GET_UV_COORDS(imageUVs), IM_COL32_WHITE);

    SetHorizontalMarqueeFade(marqueeTextMin, marqueeTextMax, Scale(32, true));
    DrawTextWithMarquee(g_pFntRodin, g_fntRodinSize, marqueeTextPos, marqueeTextMin, marqueeTextMax, IM_COL32_WHITE, g_creditsStr.c_str(), g_installerEndTime, 0.8, Scale(250, true));
    ResetMarqueeFade();
}

static void DrawMusicCredits()
{
    auto drawList = ImGui::GetBackgroundDrawList();

    constexpr auto fadeTime = COMMON_FADE_TIME;

    // Wait 12 seconds before displaying music credits,
    // as that's when the main melody begins.
    constexpr auto fadeInOffset = 738;

    // Wait 10 seconds after fade in before fading out music credits.
    constexpr auto fadeOutOffset = fadeInOffset + fadeTime + 600;

    auto fadeInMotion = ComputeLinearMotion(g_appearTime, fadeInOffset, fadeTime);
    auto fadeOutMotion = ComputeLinearMotion(g_appearTime, fadeOutOffset, fadeTime);
    auto fadeMotion = fadeInMotion - fadeOutMotion;
    auto fadeAlphaMotion = IM_COL32(0, 0, 0, 70 * fadeMotion);

    auto fontSize = Scale(12, true);
    auto offsetX = Scale(8, true);
    auto offsetY = Scale(5, true);

    if (g_aspectRatio < NARROW_ASPECT_RATIO)
        offsetY += g_vertCentre;

    drawList->AddText(g_pFntNewRodin, fontSize, { offsetX, offsetY }, fadeAlphaMotion, Localise("Installer_MusicCredits").c_str());
}

void InstallerWizard::Init()
{
    g_commonMenu = CommonMenu(Localise("Installer_Header_Installer"), "", true);

    g_installTextures[0] = LOAD_ZSTD_TEXTURE(g_install_001);
    g_installTextures[1] = LOAD_ZSTD_TEXTURE(g_install_002);
    g_installTextures[2] = LOAD_ZSTD_TEXTURE(g_install_003);
    g_installTextures[3] = LOAD_ZSTD_TEXTURE(g_install_004);
    g_installTextures[4] = LOAD_ZSTD_TEXTURE(g_install_005);
    g_installTextures[5] = LOAD_ZSTD_TEXTURE(g_install_006);
    g_installTextures[6] = LOAD_ZSTD_TEXTURE(g_install_007);
    g_installTextures[7] = LOAD_ZSTD_TEXTURE(g_install_008);
    g_upTexSonicNextDev  = LOAD_ZSTD_TEXTURE(g_sonicnextdev);

    // Add whitespace between credits for marquee.
    for (int i = 0; i < g_credits.size(); i++)
    {
        g_creditsStr += g_credits[i];
        g_creditsStr += "   ";
    }
}

void InstallerWizard::Draw()
{
    if (!s_isVisible)
        return;

    g_alphaMotion = ComputeLinearMotion(g_alphaTime, g_isIntroAnim ? COMMON_MENU_INTRO_TIME : 0, COMMON_MENU_INTRO_TIME, !g_currentMessagePrompt.empty());

    // Only offset fade motion for common menu intro.
    if (g_alphaMotion >= 1.0)
        g_isIntroAnim = false;

    auto& res = ImGui::GetIO().DisplaySize;
    auto  drawList = ImGui::GetBackgroundDrawList();

    auto min = ImVec2(g_horzCentre + g_aspectRatioNarrowMargin, g_vertCentre);
    auto max = ImVec2(res.x - min.x, res.y - min.y);
    auto isMessageWindowOpen = !g_currentMessagePrompt.empty();

    const auto bgGradientTop = IM_COL32(0, 103, 255, 255);
    const auto bgGradientBottom = IM_COL32(0, 40, 100, 255);

    drawList->AddRectFilledMultiColor({ 0.0, 0.0 }, res, bgGradientTop, bgGradientTop, bgGradientBottom, bgGradientBottom);

    // Reset cursor rects.
    g_currentCursorDefault = 0;
    g_currentCursorRects.clear();

    DrawArrows({ 0, 0 }, res, g_chevronTime);
    DrawLeftImage();

    g_commonMenu.Draw();

    DrawMusicCredits();

    auto containerWidth = Scale(640, true);
    auto containerHeight = Scale(405, true);
    auto containerOffsetY = Scale(135, true);
    auto containerMin = ImVec2(max.x - containerWidth, min.y + containerOffsetY);
    auto containerMax = ImVec2(res.x, containerMin.y + containerHeight);

    DrawContainerBox(containerMin, containerMax, g_alphaMotion);

    auto originMargin = Scale(38, true);
    auto originMin = ImVec2(containerMin.x + originMargin, containerMin.y + originMargin);
    auto originMax = ImVec2(max.x - originMargin, containerMax.y - originMargin);

    auto text = GetWizardText(g_currentPage);
    auto textMaxWidth = originMax.x - (g_fntRodinSize / 2.0f) - originMin.x;
    auto textMargin = 5.0f;
    auto textSize = MeasureCentredParagraph(g_pFntRodin, g_fntRodinSize, textMaxWidth, textMargin, text.c_str());

    if (g_currentPage == WizardPage::Installing)
    {
        DrawInstallingPage(originMin, originMax);
    }
    else
    {
        auto isNavButtonSkip = false;
        auto isNavButtonEnabled = true;
        std::function<void()> navButtonFunction = []() { SetCurrentPage(WizardPage(int(g_currentPage) + 1)); };

        switch (g_currentPage)
        {
            case WizardPage::SelectLanguage:
                DrawSelectLanguagePage(originMin, originMax);
                break;

            case WizardPage::SelectGame:
                DrawSourcePickerPage(min, max, originMin, originMax);
                isNavButtonEnabled = !g_gameSourcePath.empty();
                break;

            case WizardPage::SelectDLC:
            {
                DrawSourcePickerPage(min, max, originMin, originMax);

                isNavButtonSkip = std::all_of(g_dlcSourcePaths.begin(), g_dlcSourcePaths.end(), [](const std::filesystem::path& path) { return path.empty(); });

                navButtonFunction = [&]()
                {
                    auto isDLCInstallerMode = g_gameSourcePath.empty();

                    std::string sourcesErrorMessage{};

                    if (!InstallerParseSources(sourcesErrorMessage))
                    {
                        // Some of the sources that were provided to the installer
                        // are not valid. Restart the file selection process.
                        std::stringstream stringStream{};

                        stringStream << Localise("Installer_Message_InvalidFiles");

                        if (!sourcesErrorMessage.empty())
                            stringStream << std::endl << std::endl << sourcesErrorMessage;

                        g_currentMessagePrompt = stringStream.str();
                        g_currentMessagePromptConfirmation = false;

                        SetCurrentPage(isDLCInstallerMode ? WizardPage::SelectDLC : WizardPage::SelectGame);
                    }
                    else if (isNavButtonSkip && isDLCInstallerMode)
                    {
                        LeaveInstallerWizard();
                    }
                    else
                    {
                        SetCurrentPage(WizardPage::CheckSpace);
                    }
                };

                break;
            }

            case WizardPage::CheckSpace:
            {
                char descriptionText[512]{};
                char requiredSpaceText[128]{};
                char availableSpaceText[128]{};

                constexpr auto divisor = 1024.0 * 1024.0 * 1024.0;
                auto requiredGiB = double(g_installerSources.totalSize) / divisor;
                auto availableGiB = double(g_installerAvailableSize) / divisor;

                snprintf(requiredSpaceText, sizeof(requiredSpaceText), Localise("Installer_Step_RequiredSpace").c_str(), requiredGiB);
                snprintf(availableSpaceText, sizeof(availableSpaceText), g_installerAvailableSize > 0 ? Localise("Installer_Step_AvailableSpace").c_str() : "", availableGiB);
                snprintf(descriptionText, sizeof(descriptionText), "%s%s\n%s", text.c_str(), requiredSpaceText, availableSpaceText);

                text = descriptionText;
                navButtonFunction = []() { InstallerStart(); };

                break;
            }

            case WizardPage::InstallSucceeded:
                DrawInstallSucceededPage(originMin, originMax, textSize);
                navButtonFunction = []() { LeaveInstallerWizard(); };
                break;

            case WizardPage::InstallFailed:
                text += g_installerErrorMessage.c_str();
                navButtonFunction = []() { SetCurrentPage(g_firstPage); };
                break;
        }

        if (g_currentPage != WizardPage::SelectLanguage)
        {
            std::string navButtonText{};

            if (isNavButtonSkip)
                navButtonText = Localise("Installer_Button_Skip");
            else
                navButtonText = Localise("Installer_Button_Next");

            static auto s_isNavButtonHovered = false;

            if (DrawButton({ originMin.x, originMax.y - Scale(NAV_BUTTON_OFFSET_Y, true) }, navButtonText.c_str(), s_isNavButtonHovered, isNavButtonEnabled, true) && navButtonFunction)
                navButtonFunction();
        }
    }

    DrawTextParagraph(g_pFntRodin, g_fntRodinSize, textMaxWidth, originMin, textMargin, text.c_str(),
        [&](const char* str, ImVec2 pos) { DrawTextBasic(g_pFntRodin, g_fntRodinSize, pos, IM_COL32(255, 255, 255, 255 * g_alphaMotion), str); });

    CheckCancelAction();
    DrawMessagePrompt();
    PickerDrawForeground();
    PickerCheckTutorial();
    PickerCheckResults();
}

void InstallerWizard::Shutdown()
{
    // Wait for and reset the threads.
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

    // Free the sources.
    g_installerSources.game.reset();
    g_installerSources.dlc.clear();
    
    // Make sure the GPU is not currently active before deleting textures.
    Video::WaitForGPU();

    // Free the textures.
    for (auto &texture : g_installTextures)
        texture.reset();

    g_upTexSonicNextDev.reset();
}

bool InstallerWizard::Run(std::filesystem::path installPath, bool skipGame)
{
    g_installPath = installPath;

    EmbeddedPlayer::Init();
    NFD_Init();

    // Guarantee that one controller is initialised.
    // We'll rely on SDL's event loop to get the controller events.
    XAMINPUT_STATE inputState{};
    hid::GetState(0, &inputState);

    if (skipGame)
    {
        for (int i = 0; i < int(DLC::Count); i++)
            g_dlcInstalled[i] = Installer::checkDLCInstall(g_installPath, DLC(i + 1));

        g_firstPage = WizardPage::SelectDLC;
    }

    SetCurrentPage(g_firstPage);
    GameWindow::SetFullscreenCursorVisibility(true);

    s_isVisible = true;

    while (s_isVisible)
    {
        Video::WaitOnSwapChain();
        EmbeddedPlayer::PlayMusic();
        SDL_PumpEvents();
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        GameWindow::Update();
        Video::Present();
    }

    Fader::FadeIn(0);
    ButtonWindow::Close();
    GameWindow::SetFullscreenCursorVisibility(false);
    NFD_Quit();
    EmbeddedPlayer::Shutdown();
    InstallerWizard::Shutdown();

    return !g_isQuitting;
}
