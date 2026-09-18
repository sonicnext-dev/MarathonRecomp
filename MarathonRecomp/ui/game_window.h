#pragma once

#include <atomic>
#include <plume_render_interface_types.h>
#include <user/config.h>
#include <sdl_events.h>

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
#define MIN_WIDTH 640
#define MIN_HEIGHT 480

class GameWindow
{
public:
    static inline SDL_Window* s_pWindow = nullptr;
    static inline plume::RenderWindow s_renderWindow;

    static inline int s_x;
    static inline int s_y;
    static inline int s_width = DEFAULT_WIDTH;
    static inline int s_height = DEFAULT_HEIGHT;

    // Authoritative output size in pixels, as reported by SDL on the main thread.
    // Used to drive the render output dimensions because some backends (e.g. macOS
    // Metal) query the window size from the render thread and can return a stale
    // cached value after a resize, fullscreen toggle or monitor change.
    static inline std::atomic<uint32_t> s_pixelWidth = DEFAULT_WIDTH;
    static inline std::atomic<uint32_t> s_pixelHeight = DEFAULT_HEIGHT;

    static inline EPlayerCharacter s_playerCharacter;

    static inline bool s_isFocused;
    static inline bool s_isFullscreenCursorVisible;
    static inline bool s_isChangingDisplay;

    // State for the loop-safe auto-restart that adapts the guest render resolution to a
    // new window/output aspect ratio in EAspectRatio::Auto (see MaybeRestartForAspectChange).
    static inline bool s_aspectInitialised;
    static inline bool s_pendingAspectRestart;
    static inline double s_lastObservedAspect;
    static inline double s_pendingAspect;
    static inline uint32_t s_aspectRestartStartTicks;

    static SDL_Surface* GetIconSurface(void* pIconBmp, size_t iconSize);
    static void SetIcon(void* pIconBmp, size_t iconSize);
    static void SetIcon(EPlayerCharacter player = EPlayerCharacter::Sonic);
    static const char* GetTitle();
    static void SetTitle(const char* title = nullptr);
    static void SetTitleBarColour();
    static bool IsFullscreen();
    static bool SetFullscreen(bool isEnabled);
    static void SetFullscreenCursorVisibility(bool isVisible);
    static bool IsMaximised();
    static EWindowState SetMaximised(bool isEnabled);
    static SDL_Rect GetDimensions();
    static void GetSizeInPixels(int *w, int *h);
    static void UpdatePixelSize();
    static void SetDimensions(int w, int h, int x = SDL_WINDOWPOS_CENTERED, int y = SDL_WINDOWPOS_CENTERED);
    static void ResetDimensions();
    static uint32_t GetWindowFlags();
    static int GetDisplayCount();
    static int GetDisplay();
    static void SetDisplay(int displayIndex);
    static std::vector<SDL_DisplayMode> GetDisplayModes(bool ignoreInvalidModes = true, bool ignoreRefreshRates = true);
    static int FindNearestDisplayMode();
    static bool IsPositionValid();
    static void Init(const char* sdlVideoDriver = nullptr);
    static void Update();

private:
    // Relaunches the game to rebuild the guest render targets at the current window
    // aspect ratio when it differs from the launch aspect (Auto mode only). See the
    // implementation for the rationale and constraints.
    static void MaybeRestartForAspectChange();
};
