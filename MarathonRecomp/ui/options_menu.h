#pragma once

enum class OptionsMenuState
{
    Opening,
    Idle,
    Closing
};

class OptionsMenu
{
public:
    static inline OptionsMenuState s_state = OptionsMenuState::Opening;
    static inline bool s_isVisible = false;
    static inline bool s_isPause = false;
    static inline bool s_isRestartRequired = false;

    static void Draw();
    static void Open(bool isPause = false);
    static void Close();
    static bool CanClose();
};
