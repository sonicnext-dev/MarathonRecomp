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

// Sonicteam::GameImp::LoadingStartEnd
bool RestoreLoadingTransition(PPCRegister& r30)
{
    if (!Config::RestoreLoadingTransition)
        return false;

    auto pGameImp = (Sonicteam::GameImp*)g_memory.Translate(r30.u32);
    auto pTaskNode = &pGameImp->m_lrPopupScreenTask2;

    if (GuestToHostFunction<uint8_t>(sub_82581100, pTaskNode, 0))
    {
        auto task = GuestToHostFunction<uint32_t>(sub_82615D60, pTaskNode);

        if (task)
        {
            auto pTask = (Sonicteam::SoX::Engine::Task*)g_memory.Translate(task);

            guest_stack_var<Sonicteam::SoX::IMessage> message(0x1B000);
            pTask->ProcessMessage(message.get());
        }
    }

    return true;
}
