#pragma once

#include <api/Marathon.h>
#include <patches/hook_event.h>

class MainMenuTaskPatches
{
public:
    static inline bool s_hideButtonWindow{};
    static inline Sonicteam::MainMenuTask::MainMenuState s_state{};
    static inline std::vector<IContextHookEvent<Sonicteam::MainMenuTask>*> s_events{};
};
