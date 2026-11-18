#pragma once

#include <span>
#include <string>

enum class MessageType {
    Info,
    Error
};

class SimpleMessageBox
{
public:
    static void Show(MessageType type, const std::string& message);
};

class ChoiceMessageBox
{
public:
    static void Show(MessageType type, const std::string& message, std::span<const std::string> buttons, int* choiceOut);
};
