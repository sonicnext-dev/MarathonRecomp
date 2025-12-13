#include "loading_patches.h"
#include <api/Liberty.h>

// Sonicteam::HUDLoading::Update
PPC_FUNC_IMPL(__imp__sub_824D7340);
PPC_FUNC(sub_824D7340)
{
    auto pHUDLoading = (Sonicteam::HUDLoading*)(base + ctx.r3.u32);

    if ((pHUDLoading->m_Flags.get() & Sonicteam::HUDLoading::HUDLoadingFlags_Finished) == 0)
    {
        for (auto& event : LoadingPatches::Events)
            event->Update(ctx.f1.f64);
    }

    __imp__sub_824D7340(ctx, base);
}
