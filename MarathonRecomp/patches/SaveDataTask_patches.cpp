#include <api/Marathon.h>
#include <ui/message_window.h>
#include <user/achievement_manager.h>
#include <user/config.h>

enum
{
    ACH_ERROR_SUCCESS,
    ACH_ERROR_READ,
    ACH_ERROR_WRITE
};

static Sonicteam::SaveDataTaskXENON::SaveDataOperation g_currentAlert{};
static int g_achError{ ACH_ERROR_SUCCESS };
static int g_achErrorIgnored{ ACH_ERROR_SUCCESS };

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

    // Attempt to load achievement data on save read.
    if (ctx.r4.u32 == Sonicteam::SaveDataTaskXENON::SaveDataOperation_ReadSaveData)
    {
        if (!AchievementManager::LoadBinary())
        {
            // This condition will always be met if the achievement data is corrupted (!= IOError),
            // or as long as "Continue without saving." has not been chosen on load in this session.
            if (AchievementManager::BinStatus != EAchBinStatus::IOError || g_achErrorIgnored != ACH_ERROR_READ)
            {
                g_achError = ACH_ERROR_READ;
                return;
            }
        }
    }

    // Attempt to save achievement data on save write.
    if (ctx.r4.u32 == Sonicteam::SaveDataTaskXENON::SaveDataOperation_WriteSaveData)
    {
        // This condition will be met if the achievement data failed to save (any error status),
        // and as long as "Continue without saving." has not been chosen on save in this session.
        if (!AchievementManager::SaveBinary() && g_achErrorIgnored != ACH_ERROR_WRITE)
        {
            g_achError = ACH_ERROR_WRITE;
            return;
        }
    }

    __imp__sub_8238CB18(ctx, base);
}

// Sonicteam::SaveDataTaskXENON::Update
PPC_FUNC_IMPL(__imp__sub_8238D5C8);
PPC_FUNC(sub_8238D5C8)
{
    auto pSaveDataTaskXENON = (Sonicteam::SaveDataTaskXENON*)(base + ctx.r3.u32);

    if (g_achError != ACH_ERROR_SUCCESS)
    {
        static auto s_achErrorMessageResult = -1;

        auto& message = Localise("Title_Message_LoadAchievementDataIOError");
        std::vector<std::string> options = { Localise("Common_Retry"), Localise("Common_ContinueWithoutSaving") };

        switch (g_achError)
        {
            case ACH_ERROR_READ:
            {
                // Achievement data is unrecoverable, force user to overwrite it.
                if (AchievementManager::BinStatus != EAchBinStatus::IOError)
                {
                    message = Localise("Title_Message_LoadAchievementDataCorrupt");
                    options[0] = Localise("Common_OK");
                    options.pop_back();
                }

                break;
            }

            case ACH_ERROR_WRITE:
                message = Localise("Title_Message_SaveAchievementDataIOError");
                break;
        }

        if (MessageWindow::Open(message, &s_achErrorMessageResult, options, 0) == MSG_CLOSED)
        {
            auto operation = g_achError == ACH_ERROR_READ
                ? Sonicteam::SaveDataTaskXENON::SaveDataOperation_ReadSaveData
                : Sonicteam::SaveDataTaskXENON::SaveDataOperation_WriteSaveData;

            switch (s_achErrorMessageResult)
            {
                // Retry / OK
                case 0:
                {
                    // Create new achievement data file.
                    AchievementManager::SaveBinary(true);

                    // Display any future errors for new files.
                    g_achErrorIgnored = ACH_ERROR_SUCCESS;

                    break;
                }

                // Continue without saving.
                // Ignore any future errors unrelated to corruption for this file.
                case 1:
                    g_achErrorIgnored = g_achError;
                    break;
            }

            g_achError = ACH_ERROR_SUCCESS;
            s_achErrorMessageResult = -1;

            GuestToHostFunction<int>(sub_8238CB18, pSaveDataTaskXENON, operation);
        }
    }

    __imp__sub_8238D5C8(ctx, base);
}

void SaveAlertThreeOptionRemoveDeviceSelect(PPCRegister& r5)
{
    auto options = (uint64_t*)g_memory.Translate(r5.u32 + 8);

    // The "Select storage device." option is always the
    // second index for the three option alert windows.
    options[2] = 0;
}
