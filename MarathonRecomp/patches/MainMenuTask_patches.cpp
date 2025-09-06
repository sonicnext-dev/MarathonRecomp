#include "MainMenuTask_patches.h"

// Sonicteam::MainMenuTask::Update
PPC_FUNC_IMPL(__imp__sub_824FFCF8);
PPC_FUNC(sub_824FFCF8)
{
    auto pMainMenuTask = (Sonicteam::MainMenuTask*)(base + ctx.r3.u32);

    for (auto& event : MainMenuTaskPatches::Events)
        event->Update(pMainMenuTask, ctx.f1.f64);

    __imp__sub_824FFCF8(ctx, base);
}
