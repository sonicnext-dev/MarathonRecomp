#pragma once

#include <locale/locale.h>

struct AchievementLocale
{
    std::string Name{};
    std::string LockedDesc{};
    std::string UnlockedDesc{};
};

inline AchievementLocale g_achLocaleMissing =
{
    g_localeMissing,
    g_localeMissing,
    g_localeMissing
};

extern std::unordered_map<int, std::unordered_map<ELanguage, AchievementLocale>> g_achLocale;

AchievementLocale& GetAchievementLocale(const int key);
