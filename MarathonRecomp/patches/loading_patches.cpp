#include "loading_patches.h"
#include <app.h>

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

void DocMarathon_SetLoading_StartThread()
{
    App::s_isLoading = true;

    for (auto& event : LoadingPatches::Events)
        event->Prefix();
}

void DocMarathon_SetLoading_JoinedThread()
{
    App::s_isLoading = false;

    for (auto& event : LoadingPatches::Events)
        event->Postfix();
}
