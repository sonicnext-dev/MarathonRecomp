#pragma once

#include "hook_event.h"
#include <api/Marathon.h>

class MainMenuTaskPatches
{
public:
    static inline bool HideButtonWindow{};
    static inline Sonicteam::MainMenuTask::MainMenuState State{};
    static inline std::vector<IContextHookEvent<Sonicteam::MainMenuTask>*> Events{};
};
