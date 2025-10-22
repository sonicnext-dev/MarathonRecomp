#pragma once

#include <api/Marathon.h>
#include <ui/common_menu.h>

#define MARATHON_RECOMP_OPTIONS_MENU

enum class OptionsMenuState
{
    Opening,
    Idle,
    Closing,
    Restarting
};

enum class OptionsMenuFlowState
{
    CategoryCursor,
    OptionCursor,
    OptionSelected
};

enum class OptionsMenuCategory
{
    System,
    Input,
    Audio,
    Video,
    Debug,
    Count
};

class OptionsMenu
{
public:
    static inline CommonMenu s_commonMenu{};
    static inline OptionsMenuState s_state{};
    static inline OptionsMenuFlowState s_flowState{};
    static inline Sonicteam::SoX::Audio::Cue* s_pBgmCue{};
    static inline bool s_isVisible{};
    static inline bool s_isPause{};
    static inline bool s_isRestartRequired{};
    static inline bool s_isDebugUnlocked{};

    static void Init();
    static void Draw();
    static void Open(bool isPause = false);
    static void Close();
    static bool CanClose();
    static void SetFlowState(OptionsMenuFlowState flowState);
};
