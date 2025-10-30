#pragma once

#include <api/Marathon.h>
#include <user/config.h>

class App
{
public:
    static inline bool s_isInit;
    static inline bool s_isSkipLogos;
    static inline bool s_isMissingDLC;
    static inline bool s_isLoading;
    static inline bool s_isSaving;
    static inline bool s_isSaveDataCorrupt;

    static inline Sonicteam::AppMarathon* s_pApp;

    static inline EPlayerCharacter s_playerCharacter;
    static inline ELanguage s_language;

    static inline double s_deltaTime;
    static inline double s_time = 0.0; // How much time elapsed since the game started.

    static void Restart(std::vector<std::string> restartArgs = {});
    static void Exit();
};

