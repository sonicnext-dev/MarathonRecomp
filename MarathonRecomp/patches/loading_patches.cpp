#include "loading_patches.h"
#include <api/Marathon.h>
#include <user/config.h>
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
    const auto pHUDLoading = static_cast<Sonicteam::HUDLoading*>(reinterpret_cast<Sonicteam::SoX::Engine::Task*>(base + ctx.r3.u32));

    if (Config::RestoreLoadingTransition)
    {
        // Fix loading CSD priority in GameMode.
        if (pHUDLoading->GetDoc<Sonicteam::DocMarathonState>()->GetDocMode("GameMode"))
            pHUDLoading->m_spCsdObject->m_Priority = 1002.0f;
    }

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

    // Prevent LoadingTask from being deleted here, as
    // the game deletes it when the outro animation ends.
    return true;
}
