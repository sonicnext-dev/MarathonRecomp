#include <api/Marathon.h>
#include <user/config.h>

#include <os/logger.h>

static Sonicteam::SaveDataTaskXENON::SaveDataOperation g_currentAlert{};

// Sonicteam::SaveDataTaskXENON::RunOperation
PPC_FUNC_IMPL(__imp__sub_8238CB18);
PPC_FUNC(sub_8238CB18)
{
    // Redirect storage device lost alert to load failed alert.
    // The user should never see this alert, but if they do, it should make a bit more sense.
    if (ctx.r4.u32 == Sonicteam::SaveDataTaskXENON::eSaveDataOperation_AlertSelectDevice)
        ctx.r4.u32 = Sonicteam::SaveDataTaskXENON::eSaveDataOperation_AlertLoadFailed;

    // Update current alert to operations that only display
    // messages so we can keep track of the last message displayed.
    if (ctx.r4.u32 < 3 && ctx.r4.u32 >= 6 && ctx.r4.u32 < 8)
        g_currentAlert = (Sonicteam::SaveDataTaskXENON::SaveDataOperation)ctx.r4.u32;

    // Redirect overwrite alert to just save the game, if requested.
    if (Config::Autosave && ctx.r4.u32 == Sonicteam::SaveDataTaskXENON::eSaveDataOperation_AlertOverwrite)
        ctx.r4.u32 = Sonicteam::SaveDataTaskXENON::eSaveDataOperation_WriteSaveData;

    // Redirect storage device select operation to access the save data.
    // This option is replaced with a "Retry" option, so it should attempt to access it again.
    if (ctx.r4.u32 == Sonicteam::SaveDataTaskXENON::eSaveDataOperation_SelectStorageDevice)
    {
        if (g_currentAlert == Sonicteam::SaveDataTaskXENON::eSaveDataOperation_AlertLoadFailed)
        {
            ctx.r4.u32 = Sonicteam::SaveDataTaskXENON::eSaveDataOperation_ReadSaveData;
        }
        else
        {
            ctx.r4.u32 = Sonicteam::SaveDataTaskXENON::eSaveDataOperation_WriteSaveData;
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

// Load system message.
PPC_FUNC_IMPL(__imp__sub_825ECB48);
PPC_FUNC(sub_825ECB48)
{
    auto* pMessage = (const char*)(base + ctx.r5.u32);
    void* pNewMessage = nullptr;

    if (strcmp(pMessage, "msg_deviceselect") == 0)
    {
        constexpr const char* MSG_RETRY = "msg_retry";
    
        pNewMessage = g_userHeap.Alloc(strlen(MSG_RETRY) + 1);
    
        strcpy((char*)pNewMessage, MSG_RETRY);
    
        ctx.r5.u32 = g_memory.MapVirtual(pNewMessage);
    }

    __imp__sub_825ECB48(ctx, base);

    if (!pNewMessage)
        return;

    g_userHeap.Free(pNewMessage);
}
