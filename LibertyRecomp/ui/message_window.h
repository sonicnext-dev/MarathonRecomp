#pragma once

#define MSG_OPEN   (false)
#define MSG_CLOSED (true)

class MessageWindow
{
public:
    static inline bool s_isVisible = false;

    static void Draw();
    static bool Open(std::string text, int* result, std::span<std::string> buttons = {}, int defaultButtonIndex = 0, int cancelButtonIndex = 1);
    static void Close();
};
