#include "loading_patches.h"
#include <app.h>

void DocMarathon_SetLoading_StartThread()
{
    App::s_isLoading = true;

    for (auto& event : LoadingPatches::Events)
        event->Prefix();
}

// Sonicteam::HUDLoading::Update
PPC_FUNC_IMPL(__imp__sub_824D7340);
PPC_FUNC(sub_824D7340)
{
    if (App::s_isLoading)
    {
        for (auto& event : LoadingPatches::Events)
            event->Update(ctx.f1.f64);
    }

    __imp__sub_824D7340(ctx, base);
}

void DocMarathon_SetLoading_JoinThread_Prefix()
{
    // Run postfix events before joining to allow the loading
    // animation to continue, if more work needs to be done.
    for (auto& event : LoadingPatches::Events)
        event->Postfix();
}

void DocMarathon_SetLoading_JoinThread_Postfix()
{
    App::s_isLoading = false;
}
