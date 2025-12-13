#pragma once

class ButtonWindow
{
public:
    static inline bool s_isVisible = false;

    static void Draw();
    static void Open(std::string key, bool isAnimated = true);
    static void Close();
};
