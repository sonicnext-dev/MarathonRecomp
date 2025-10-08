#pragma once

enum class EButtonIcon
{
    // Controller
    A,
    B,
    X,
    Y,
    LB,
    RB,
    LBRB,
    LT,
    RT,
    LTRT,
    Start,
    Back,

    // Keyboard + Mouse (temporary) <-- lol
    LMB,
    Enter,
    Escape
};

class Button
{
public:
    std::string Name{};
    EButtonIcon Icon{};
    bool* Visibility{ nullptr };

    Button(std::string name, EButtonIcon icon, bool* visibility = nullptr)
        : Name(name), Icon(icon), Visibility(visibility) {}
};

class ButtonGuide
{
public:
    static inline bool s_isVisible = false;

    static void Init();
    static void Draw();
    static void Open(Button button);
    static void Open(const std::span<Button> buttons);
    static void Close();
};
