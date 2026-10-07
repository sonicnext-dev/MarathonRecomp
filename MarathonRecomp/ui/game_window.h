#pragma once

#include <plume_render_interface_types.h>
#include <user/config.h>
#include <sdl_events.h>

class GameWindow
{
public:
    static constexpr int k_defaultWidth = 1280;
    static constexpr int k_defaultHeight = 720;
    static constexpr int k_minWidth = 640;
    static constexpr int k_minHeight = 480;

    static inline SDL_Window* s_pWindow = nullptr;
    static inline plume::RenderWindow s_renderWindow;

    static inline int s_x;
    static inline int s_y;
    static inline int s_width = k_defaultWidth;
    static inline int s_height = k_defaultHeight;

    static inline std::vector<SDL_DisplayMode> s_windowModes{};

    static inline EPlayerCharacter s_playerCharacter;

    static inline bool s_isFocused;
    static inline bool s_isFullscreenCursorVisible;
    static inline bool s_isChangingDisplay;

    static SDL_Surface* GetIconSurface(void* pIconBmp, size_t iconSize);
    static void SetIcon(void* pIconBmp, size_t iconSize);
    static void SetIcon(EPlayerCharacter player = EPlayerCharacter::Sonic);
    static const char* GetTitle();
    static void SetTitle(const char* title = nullptr);
    static void ConfigurePlatformWindow();
    static bool IsFullscreen();
    static bool SetFullscreen(bool isEnabled);
    static void SetFullscreenCursorVisibility(bool isVisible);
    static bool IsMaximised();
    static EWindowState SetMaximised(bool isEnabled);
    static SDL_Rect GetDimensions();
    static void GetSizeInPixels(int *w, int *h);
    static void SetDimensions(int w, int h, int x = SDL_WINDOWPOS_CENTERED, int y = SDL_WINDOWPOS_CENTERED);
    static void ResetDimensions();
    static uint64_t GetWindowFlags();
    static int GetDisplayCount();
    static int GetDisplay();
    static void SetDisplay(int displayIndex);
    static std::vector<SDL_DisplayMode> GetDisplayModes(bool ignoreInvalidModes = true, bool ignoreRefreshRates = true);
    static int FindNearestDisplayMode(const std::vector<SDL_DisplayMode>& displayModes);
    static int FindNearestDisplayMode(bool ignoreInvalidModes = true, bool ignoreRefreshRates = true);
    static int FindNearestWindowMode();
    static bool IsPositionValid();
    static void Init(const char* sdlVideoDriver = nullptr);
    static void Update();
};
