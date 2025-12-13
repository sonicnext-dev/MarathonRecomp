#include <api/Liberty.h>
#include <user/config.h>

static Sonicteam::SaveDataTaskXENON::SaveDataOperation g_currentAlert{};

// Sonicteam::SaveDataTaskXENON::RunOperation (speculatory)
PPC_FUNC_IMPL(__imp__sub_8238CB18);
PPC_FUNC(sub_8238CB18)
{
    // Redirect storage device lost alert to load failed alert.
    // The user should never see this alert, but if they do, it should make a bit more sense.
    if (ctx.r4.u32 == Sonicteam::SaveDataTaskXENON::SaveDataOperation_AlertSelectDevice)
        ctx.r4.u32 = Sonicteam::SaveDataTaskXENON::SaveDataOperation_AlertLoadFailed;

    // Update current alert to operations that only display
    // messages so we can keep track of the last message displayed.
    if (ctx.r4.u32 < 3 && ctx.r4.u32 >= 6 && ctx.r4.u32 < 8)
        g_currentAlert = (Sonicteam::SaveDataTaskXENON::SaveDataOperation)ctx.r4.u32;

    // Redirect overwrite alert to just save the game, if requested.
    if (Config::Autosave && ctx.r4.u32 == Sonicteam::SaveDataTaskXENON::SaveDataOperation_AlertOverwrite)
        ctx.r4.u32 = Sonicteam::SaveDataTaskXENON::SaveDataOperation_WriteSaveData;

    // Redirect storage device select operation to access the save data.
    // This option is replaced with a "Retry" option, so it should attempt to access it again.
    if (ctx.r4.u32 == Sonicteam::SaveDataTaskXENON::SaveDataOperation_SelectStorageDevice)
    {
        if (g_currentAlert == Sonicteam::SaveDataTaskXENON::SaveDataOperation_AlertLoadFailed)
        {
            ctx.r4.u32 = Sonicteam::SaveDataTaskXENON::SaveDataOperation_ReadSaveData;
        }
        else
        {
            ctx.r4.u32 = Sonicteam::SaveDataTaskXENON::SaveDataOperation_WriteSaveData;
        }
    }

    __imp__sub_8238CB18(ctx, base);
}

void SaveAlertThreeOptionRemoveDeviceSelect(PPCRegister& r5)
{
    auto options = (uint64_t*)g_memory.Translate(r5.u32 + 8);

    // The "Select storage device." option is always the
    // second index for the three option alert windows.
    options[2] = 0;
}
