#pragma once

#include "hook_event.h"
#include <api/Liberty.h>

class MainMenuTaskPatches
{
public:
    static inline Sonicteam::MainMenuTask::MainMenuState State{};
    static inline std::vector<IContextHookEvent<Sonicteam::MainMenuTask>*> Events{};
};
