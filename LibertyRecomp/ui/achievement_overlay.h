#pragma once

#include <queue>

class AchievementOverlay
{
public:
    static inline bool s_isVisible = false;

    static inline std::queue<uint16_t> s_queue{};

    static void Init();
    static void Draw();
    static void Open(int id);
    static void Close();
};
