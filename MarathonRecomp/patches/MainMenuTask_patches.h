#pragma once

#include "hook_event.h"
#include <api/Marathon.h>

class MainMenuTaskPatches
{
public:
    static inline std::vector<IContextHookEvent<Sonicteam::MainMenuTask>*> Events{};
};
