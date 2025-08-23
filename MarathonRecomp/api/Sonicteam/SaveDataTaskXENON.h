#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class SaveDataTaskXENON : public SaveDataTask
    {
    public:
        enum SaveDataOperation : uint32_t
        {
            SaveDataOperation_AlertOverwrite,
            SaveDataOperation_AlertNoSaveData,
            SaveDataOperation_AlertSelectDevice,
            SaveDataOperation_SelectStorageDevice,
            SaveDataOperation_WriteSaveData,
            SaveDataOperation_ReadSaveData,
            SaveDataOperation_AlertSaveFailed,
            SaveDataOperation_AlertLoadFailed,
            SaveDataOperation_8,
            SaveDataOperation_9
        };

        xpointer<AlertWindowTask> m_pAlertWindowTask;
        MARATHON_INSERT_PADDING(8);
        bool m_IsAccessSuccess;
        xpointer<void> m_pTextBook;
        be<SaveDataOperation> m_Operation;
    };
}
