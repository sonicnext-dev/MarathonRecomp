#include "loading_patches.h"
#include <api/Marathon.h>
#include <user/config.h>

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

bool RestoreLoadingTransition(PPCRegister& r30)
{
    if (!Config::RestoreLoadingTransition)
        return false;

    const auto pGame = reinterpret_cast<Sonicteam::GameImp*>(g_memory.Translate(r30.u32));

    if (const auto pLoadingTask = pGame->m_lrLoadingTask.m_pElement)
    {
        // Send message to LoadingTask to advance HUDLoading animation.
        guest_stack_var<Sonicteam::SoX::IMessage> msg(0x1B000);
        pLoadingTask->ProcessMessage(msg.get());
    }

    // Prevent Sonicteam::LoadingTask from being destroyed.
    // TODO: ensure this doesn't cause a leak.
    return true;
}
