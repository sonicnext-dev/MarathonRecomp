#pragma once

#include <ui/common_menu.h>

enum class OptionsMenuState
{
    Opening,
    Idle,
    Closing
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
    Code,
    Count
};

class OptionsMenu
{
public:
    static inline CommonMenu s_commonMenu{};
    static inline OptionsMenuState s_state{};
    static inline OptionsMenuFlowState s_flowState{};
    static inline bool s_isVisible{};
    static inline bool s_isPause{};
    static inline bool s_isRestartRequired{};
    static inline bool s_isCodesUnlocked{};

    static void Init();
    static void Draw();
    static void Open(bool isPause = false);
    static void Close();
    static bool CanClose();
};
