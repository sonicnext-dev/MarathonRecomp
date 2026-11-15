#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>

enum class MessageType {
    Info,
    Error
};

class MessageBox
{
public:
    static void ShowSimple(MessageType type, const std::string& message);
    static void ShowChoice(MessageType type, const std::string& message, std::span<const std::string> buttons, int* choiceOut);
};
