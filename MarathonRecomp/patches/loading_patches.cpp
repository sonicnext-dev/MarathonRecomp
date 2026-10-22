#include "loading_patches.h"
#include <api/Marathon.h>
#include <user/config.h>
#include <app.h>

Sonicteam::LoadingTask* GetLoadingTask(PPCRegister& r31)
{
    const auto pDoc = reinterpret_cast<Sonicteam::DocMarathonState*>(g_memory.Translate(r31.u32));

    return pDoc->m_pRootTask->Find<Sonicteam::LoadingTask>("LoadingTask");
}

void DocMarathon_SetLoading_StartThread(PPCRegister& r31)
{
    App::s_isLoading = true;

    const auto pLoadingTask = GetLoadingTask(r31);

    if (!pLoadingTask)
        return;

    for (auto& event : LoadingPatches::Events)
        event->Prefix(pLoadingTask);
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
            event->Update(pHUDLoading->GetParent<Sonicteam::LoadingTask>(), ctx.f1.f64);
    }

    __imp__sub_824D7340(ctx, base);
}

void DocMarathon_SetLoading_JoinThread_Prefix(PPCRegister& r31)
{
    const auto pLoadingTask = GetLoadingTask(r31);

    if (!pLoadingTask)
        return;

    // Run postfix events before joining to allow the loading
    // animation to continue, if more work needs to be done.
    for (auto& event : LoadingPatches::Events)
        event->Postfix(pLoadingTask);
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

    if (const auto pLoadingTask = pGame->m_rLoadingTask.m_pRef)
    {
        // Send message to LoadingTask to advance HUDLoading animation.
        guest_stack_var<Sonicteam::SoX::IMessage> msg(0x1B000);
        pLoadingTask->ProcessMessage(msg.get());
    }

    // Prevent LoadingTask from being deleted here, as
    // the game deletes it when the outro animation ends.
    return true;
}
