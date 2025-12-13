#pragma once

class Fader
{
public:
    static inline bool s_isVisible = false;

    static void Draw();
    static void SetFadeColour(ImU32 colour);
    static void FadeIn(float duration = 1, std::function<void()> endCallback = nullptr, float endCallbackDelay = 0.75f);
    static void FadeOut(float duration = 1, std::function<void()> endCallback = nullptr, float endCallbackDelay = 0.75f);
};
