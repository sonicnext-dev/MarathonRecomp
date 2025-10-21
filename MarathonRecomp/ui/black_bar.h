#pragma once

class BlackBar
{
public:
    static constexpr double ms_MenuBorderMargin = 70.0;

    static inline float s_pillarboxWidth{};
    static inline float s_letterboxHeight{};
    static inline float s_margin{};

    static void Draw();
    static void Show(bool isEdgeFade = false);
    static void Hide();
    static void SetBorderMargin(float margin);
    static bool IsVisible();
};
