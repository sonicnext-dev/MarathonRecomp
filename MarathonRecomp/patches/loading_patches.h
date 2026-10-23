#pragma once

#include <Marathon.h>
#include <patches/hook_event.h>

class LoadingPatches
{
public:
    static inline std::vector<IContextHookEvent<Sonicteam::LoadingTask>*> Events{};
};
