#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class SaveDataTaskXENON : public SaveDataTask
    {
    public:
        enum SaveDataOperation : uint32_t
        {
            eSaveDataOperation_AlertOverwrite,
            eSaveDataOperation_AlertNoSaveData,
            eSaveDataOperation_AlertSelectDevice,
            eSaveDataOperation_SelectStorageDevice,
            eSaveDataOperation_WriteSaveData,
            eSaveDataOperation_ReadSaveData,
            eSaveDataOperation_AlertSaveFailed,
            eSaveDataOperation_AlertLoadFailed,
            eSaveDataOperation_8,
            eSaveDataOperation_9
        };

        xpointer<AlertWindowTask> m_pAlertWindowTask;
        MARATHON_INSERT_PADDING(8);
        bool m_IsAccessSuccess;
        xpointer<void> m_pTextBook;
        be<SaveDataOperation> m_Operation;
    };
}
