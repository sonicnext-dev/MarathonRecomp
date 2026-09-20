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

// Sonicteam::GameImp::LoadingStartEnd
bool RestoreLoadingTransition(PPCRegister& r30)
{
    if (!Config::RestoreLoadingTransition)
        return false;

    auto pGameImp = (Sonicteam::GameImp*)g_memory.Translate(r30.u32);
    auto pTaskNode = &pGameImp->m_GamePopupScreenTask2;

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
