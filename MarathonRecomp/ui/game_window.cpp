#include "game_window.h"
#include <gpu/video.h>
#include <os/logger.h>
#include <os/user.h>
#include <os/version.h>
#include <app.h>
#include <sdl_listener.h>

#if _WIN32
#include <dwmapi.h>
#include <shellscalingapi.h>
#endif

#include <res/images/game_icon.bmp.h>

bool m_isFullscreenKeyReleased = true;
bool m_isResizing = false;

static SDL_DisplayID GetDisplayID(int displayIndex)
{
    int displayCount = 0;
    auto displays = SDL_GetDisplays(&displayCount);
    SDL_DisplayID result = 0;

    if (displays)
    {
        if (displayIndex >= 0 && displayIndex < displayCount)
            result = displays[displayIndex];

        SDL_free(displays);
    }

    return result;
}

bool Window_OnSDLEvent(void*, SDL_Event* event)
{
    if (ImGui::GetIO().BackendPlatformUserData != nullptr)
        ImGui_ImplSDL3_ProcessEvent(event);

    for (auto listener : GetEventListeners())
    {
        if (listener->OnSDLEvent(event))
        {
            return true;
        }
    }

    switch (event->type)
    {
        case SDL_EVENT_QUIT:
        {
            if (App::s_isSaving)
                break;

            App::Exit();

            break;
        }

        case SDL_EVENT_KEY_DOWN:
        {
            switch (event->key.key)
            {
                // Toggle fullscreen on ALT+ENTER.
                case SDLK_RETURN:
                {
                    if (!(event->key.mod & SDL_KMOD_ALT) || !m_isFullscreenKeyReleased)
                        break;

                    Config::Fullscreen = GameWindow::SetFullscreen(!GameWindow::IsFullscreen());

                    if (Config::Fullscreen)
                    {
                        Config::Monitor = GameWindow::GetDisplay();
                    }
                    else
                    {
                        Config::WindowState = GameWindow::SetMaximised(Config::WindowState == EWindowState::Maximised);
                    }

                    // Block holding ALT+ENTER spamming window changes.
                    m_isFullscreenKeyReleased = false;

                    break;
                }

                // Restore original window dimensions on F2.
                case SDLK_F2:
                    Config::Fullscreen = GameWindow::SetFullscreen(false);
                    GameWindow::ResetDimensions();
                    break;

                // Recentre window on F3.
                case SDLK_F3:
                {
                    if (GameWindow::IsFullscreen())
                        break;

                    GameWindow::SetDimensions(GameWindow::s_width, GameWindow::s_height);

                    break;
                }
            }

            break;
        }

        case SDL_EVENT_KEY_UP:
        {
            switch (event->key.key)
            {
                // Allow user to input ALT+ENTER again.
                case SDLK_RETURN:
                    m_isFullscreenKeyReleased = true;
                    break;
            }

            break;
        }

        case SDL_EVENT_WINDOW_FOCUS_LOST:
            GameWindow::s_isFocused = false;
            SDL_ShowCursor();
            break;

        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        {
            GameWindow::s_isFocused = true;

            if (GameWindow::IsFullscreen())
            {
                if (GameWindow::s_isFullscreenCursorVisible)
                    SDL_ShowCursor();
                else
                    SDL_HideCursor();
            }

            break;
        }

        case SDL_EVENT_WINDOW_RESTORED:
            Config::WindowState = EWindowState::Normal;
            break;

        case SDL_EVENT_WINDOW_MAXIMIZED:
            Config::WindowState = EWindowState::Maximised;
            break;

        case SDL_EVENT_WINDOW_RESIZED:
            m_isResizing = true;
            Config::WindowSize = -1;
            GameWindow::s_width = event->window.data1;
            GameWindow::s_height = event->window.data2;
            GameWindow::SetTitle(fmt::format("{} - [{}x{}]", GameWindow::GetTitle(), GameWindow::s_width, GameWindow::s_height).c_str());
            break;

        case SDL_EVENT_WINDOW_MOVED:
            GameWindow::s_x = event->window.data1;
            GameWindow::s_y = event->window.data2;
            break;

        case SDL_EVENT_USER_PLAYER_CHAR:
            GameWindow::s_playerCharacter = static_cast<EPlayerCharacter>(event->user.code);
            GameWindow::SetIcon(GameWindow::s_playerCharacter);
            break;
    }

    return false;
}

void GameWindow::Init(const char* sdlVideoDriver)
{
#ifdef __linux__
    SDL_SetHint(SDL_HINT_APP_ID, "io.github.sonicnext_dev.marathonrecomp");
#endif

    if (sdlVideoDriver)
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, sdlVideoDriver);

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO) && sdlVideoDriver)
    {
        LOGFN_ERROR("Failed to initialise the SDL video driver: \"{}\". Falling back to default.", sdlVideoDriver);
        SDL_ResetHint(SDL_HINT_VIDEO_DRIVER);
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    }

    auto videoDriverName = SDL_GetCurrentVideoDriver();

    if (videoDriverName)
        LOGFN("SDL video driver: \"{}\"", videoDriverName);

    SDL_AddEventWatch(Window_OnSDLEvent, s_pWindow);

#ifdef _WIN32
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
#endif

    s_x = Config::WindowX;
    s_y = Config::WindowY;
    s_width = Config::WindowWidth;
    s_height = Config::WindowHeight;

    if (s_x == -1 && s_y == -1)
        s_x = s_y = SDL_WINDOWPOS_CENTERED;

    if (!IsPositionValid())
        GameWindow::ResetDimensions();

    s_pWindow = SDL_CreateWindow("Marathon Recompiled", s_width, s_height, GetWindowFlags());
    SDL_SetWindowPosition(s_pWindow, s_x, s_y);

    if (IsFullscreen())
        SDL_HideCursor();

    SetDisplay(Config::Monitor);
    SetIcon();
    SetTitle();

    SDL_SetWindowMinimumSize(s_pWindow, MIN_WIDTH, MIN_HEIGHT);

#if defined(_WIN32)
    s_renderWindow = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(s_pWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

    if (Config::DisableDWMRoundedCorners)
    {
        DWM_WINDOW_CORNER_PREFERENCE wcp = DWMWCP_DONOTROUND;
        DwmSetWindowAttribute(s_renderWindow, DWMWA_WINDOW_CORNER_PREFERENCE, &wcp, sizeof(wcp));
    }
#elif defined(PLUME_SDL_VULKAN_ENABLED)
    s_renderWindow = s_pWindow;
#elif defined(__linux__)
    s_renderWindow = {
        (Display*)SDL_GetPointerProperty(SDL_GetWindowProperties(s_pWindow), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr),
        (Window)SDL_GetNumberProperty(SDL_GetWindowProperties(s_pWindow), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0)
    };
#elif defined(__APPLE__)
    s_renderWindow.window = SDL_GetPointerProperty(SDL_GetWindowProperties(s_pWindow), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
    s_renderWindow.view = SDL_Metal_GetLayer(SDL_Metal_CreateView(s_pWindow));
#else
    static_assert(false, "Unknown platform.");
#endif

    SetTitleBarColour();

    SDL_ShowWindow(s_pWindow);
}

void GameWindow::Update()
{
    if (!GameWindow::IsFullscreen() && !GameWindow::IsMaximised() && !s_isChangingDisplay)
    {
        Config::WindowX = GameWindow::s_x;
        Config::WindowY = GameWindow::s_y;
        Config::WindowWidth = GameWindow::s_width;
        Config::WindowHeight = GameWindow::s_height;
    }

    if (m_isResizing)
    {
        SetTitle();
        m_isResizing = false;
    }

    if (g_needsResize)
        s_isChangingDisplay = false;
}

SDL_Surface* GameWindow::GetIconSurface(void* pIconBmp, size_t iconSize)
{
    auto io = SDL_IOFromMem(pIconBmp, iconSize);
    auto surface = SDL_LoadBMP_IO(io, true);

    if (!surface)
        LOGF_ERROR("Failed to load icon: {}", SDL_GetError());

    return surface;
}

void GameWindow::SetIcon(void* pIconBmp, size_t iconSize)
{
    if (auto icon = GetIconSurface(pIconBmp, iconSize))
    {
        SDL_SetWindowIcon(s_pWindow, icon);
        SDL_DestroySurface(icon);
    }
}

void GameWindow::SetIcon(EPlayerCharacter player)
{
    // TODO: Per-character icons
    switch (player) {
        case EPlayerCharacter::Sonic:
            break;
        case EPlayerCharacter::Shadow:
            break;
        case EPlayerCharacter::Silver:
            break;
        case EPlayerCharacter::Blaze:
            break;
        case EPlayerCharacter::Amy:
            break;
        case EPlayerCharacter::Tails:
            break;
        case EPlayerCharacter::Rouge:
            break;
        case EPlayerCharacter::Knuckles:
            break;
    }

    SetIcon(g_game_icon, sizeof(g_game_icon));
}

const char* GameWindow::GetTitle()
{
    if (Config::UseOfficialTitleOnTitleBar)
    {
        return "SONIC THE HEDGEHOG";
    }

    return "Marathon Recompiled";
}

void GameWindow::SetTitle(const char* title)
{
    SDL_SetWindowTitle(s_pWindow, title ? title : GetTitle());
}

void GameWindow::SetTitleBarColour()
{
#if _WIN32
    if (os::user::IsDarkTheme())
    {
        auto version = os::version::GetOSVersion();

        if (version.Major < 10 || version.Build <= 17763)
            return;

        auto flag = version.Build >= 18985
            ? DWMWA_USE_IMMERSIVE_DARK_MODE
            : 19; // DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1

        const DWORD useImmersiveDarkMode = 1;
        DwmSetWindowAttribute(s_renderWindow, flag, &useImmersiveDarkMode, sizeof(useImmersiveDarkMode));
    }
#endif
}

bool GameWindow::IsFullscreen()
{
    return SDL_GetWindowFlags(s_pWindow) & SDL_WINDOW_FULLSCREEN;
}

bool GameWindow::SetFullscreen(bool isEnabled)
{
    if (isEnabled)
    {
        SDL_SetWindowFullscreenMode(s_pWindow, nullptr);
        SDL_SetWindowFullscreen(s_pWindow, true);

        if (s_isFullscreenCursorVisible)
            SDL_ShowCursor();
        else
            SDL_HideCursor();
    }
    else
    {
        SDL_SetWindowFullscreen(s_pWindow, false);
        SDL_ShowCursor();

        SetIcon(GameWindow::s_playerCharacter);
        SetDimensions(Config::WindowWidth, Config::WindowHeight, Config::WindowX, Config::WindowY);
    }

    return isEnabled;
}

void GameWindow::SetFullscreenCursorVisibility(bool isVisible)
{
    s_isFullscreenCursorVisible = isVisible;

    if (IsFullscreen() && !s_isFullscreenCursorVisible)
    {
        SDL_HideCursor();
    }
    else
    {
        SDL_ShowCursor();
    }
}

bool GameWindow::IsMaximised()
{
    return SDL_GetWindowFlags(s_pWindow) & SDL_WINDOW_MAXIMIZED;
}

EWindowState GameWindow::SetMaximised(bool isEnabled)
{
    if (isEnabled)
    {
        SDL_MaximizeWindow(s_pWindow);
    }
    else
    {
        SDL_RestoreWindow(s_pWindow);
    }

    return isEnabled
        ? EWindowState::Maximised
        : EWindowState::Normal;
}

SDL_Rect GameWindow::GetDimensions()
{
    SDL_Rect rect{};

    SDL_GetWindowPosition(s_pWindow, &rect.x, &rect.y);
    SDL_GetWindowSize(s_pWindow, &rect.w, &rect.h);

    return rect;
}

void GameWindow::GetSizeInPixels(int *w, int *h)
{
    SDL_GetWindowSizeInPixels(s_pWindow, w, h);
}

void GameWindow::SetDimensions(int w, int h, int x, int y)
{
    s_width = w;
    s_height = h;
    s_x = x;
    s_y = y;

    SDL_SetWindowSize(s_pWindow, w, h);
    SDL_ResizeEvent(s_pWindow, w, h);

    SDL_SetWindowPosition(s_pWindow, x, y);
    SDL_MoveEvent(s_pWindow, x, y);
}

void GameWindow::ResetDimensions()
{
    s_x = SDL_WINDOWPOS_CENTERED;
    s_y = SDL_WINDOWPOS_CENTERED;
    s_width = DEFAULT_WIDTH;
    s_height = DEFAULT_HEIGHT;

    Config::WindowX = s_x;
    Config::WindowY = s_y;
    Config::WindowWidth = s_width;
    Config::WindowHeight = s_height;
}

SDL_WindowFlags GameWindow::GetWindowFlags()
{
    SDL_WindowFlags flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (Config::WindowState == EWindowState::Maximised)
        flags |= SDL_WINDOW_MAXIMIZED;

    if (Config::Fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;

#ifdef PLUME_SDL_VULKAN_ENABLED
    flags |= SDL_WINDOW_VULKAN;
#endif

    return flags;
}

int GameWindow::GetDisplayCount()
{
    int result = 0;
    auto displays = SDL_GetDisplays(&result);

    if (!displays)
    {
        LOGF_ERROR("Failed to get display count: {}", SDL_GetError());
        return 1;
    }

    SDL_free(displays);

    return result;
}

int GameWindow::GetDisplay()
{
    auto displayID = SDL_GetDisplayForWindow(s_pWindow);
    int displayCount = 0;
    auto displays = SDL_GetDisplays(&displayCount);
    int result = -1;

    if (displays)
    {
        for (int i = 0; i < displayCount; i++)
        {
            if (displays[i] == displayID)
            {
                result = i;
                break;
            }
        }

        SDL_free(displays);
    }

    return result;
}

void GameWindow::SetDisplay(int displayIndex)
{
    if (!IsFullscreen())
        return;

    if (GetDisplay() == displayIndex)
        return;

    s_isChangingDisplay = true;

    SDL_Rect bounds;

    if (SDL_GetDisplayBounds(GetDisplayID(displayIndex), &bounds))
    {
        SetFullscreen(false);
        SetDimensions(bounds.w, bounds.h, bounds.x, bounds.y);
        SetFullscreen(true);
    }
    else
    {
        ResetDimensions();
    }
}

std::vector<SDL_DisplayMode> GameWindow::GetDisplayModes(bool ignoreInvalidModes, bool ignoreRefreshRates)
{
    auto result = std::vector<SDL_DisplayMode>();
    auto uniqueResolutions = std::set<std::pair<int, int>>();
    auto displayID = GetDisplayID(GetDisplay());
    auto modeCount = 0;
    auto modes = SDL_GetFullscreenDisplayModes(displayID, &modeCount);

    if (!modes)
        return result;

    for (int i = modeCount - 1; i >= 0; i--)
    {
        const SDL_DisplayMode& mode = *modes[i];

        if (ignoreInvalidModes)
        {
            if (mode.w < MIN_WIDTH || mode.h < MIN_HEIGHT)
                continue;

            auto desktopMode = SDL_GetDesktopDisplayMode(displayID);

            if (desktopMode && (mode.w >= desktopMode->w || mode.h >= desktopMode->h))
                continue;
        }

        if (ignoreRefreshRates)
        {
            auto res = std::make_pair(mode.w, mode.h);

            if (uniqueResolutions.find(res) == uniqueResolutions.end())
            {
                uniqueResolutions.insert(res);
                result.push_back(mode);
            }
        }
        else
        {
            result.push_back(mode);
        }
    }

    SDL_free(modes);

    return result;
}

int GameWindow::FindNearestDisplayMode()
{
    auto result = -1;
    auto displayModes = GetDisplayModes();
    auto currentDiff = std::numeric_limits<int>::max();

    for (int i = 0; i < displayModes.size(); i++)
    {
        auto& mode = displayModes[i];

        auto widthDiff = abs(mode.w - s_width);
        auto heightDiff = abs(mode.h - s_height);
        auto totalDiff = widthDiff + heightDiff;

        if (totalDiff < currentDiff)
        {
            currentDiff = totalDiff;
            result = i;
        }
    }

    return result;
}

bool GameWindow::IsPositionValid()
{
    int displayCount = 0;
    auto displays = SDL_GetDisplays(&displayCount);

    if (!displays)
        return false;

    auto result = false;

    for (int i = 0; i < displayCount; i++)
    {
        SDL_Rect bounds;

        if (!SDL_GetDisplayBounds(displays[i], &bounds))
            continue;

        auto x = s_x;
        auto y = s_y;

        // Window spans across the entire display in windowed mode, which is invalid.
        if (!Config::Fullscreen && s_width == bounds.w && s_height == bounds.h)
            break;

        if (x == SDL_WINDOWPOS_CENTERED_DISPLAY(displays[i]))
            x = bounds.w / 2 - s_width / 2;

        if (y == SDL_WINDOWPOS_CENTERED_DISPLAY(displays[i]))
            y = bounds.h / 2 - s_height / 2;

        if (x >= bounds.x && x < bounds.x + bounds.w &&
            y >= bounds.y && y < bounds.y + bounds.h)
        {
            result = true;
            break;
        }
    }

    SDL_free(displays);

    return result;
}
