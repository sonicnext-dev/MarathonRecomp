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
        xpointer<void> m_pSystemTextBook;
        be<SaveDataOperation> m_Operation;
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_OFFSETOF(SaveDataTaskXENON, m_pAlertWindowTask, 0x7C);
    MARATHON_ASSERT_OFFSETOF(SaveDataTaskXENON, m_IsAccessSuccess, 0x88);
    MARATHON_ASSERT_OFFSETOF(SaveDataTaskXENON, m_pSystemTextBook, 0x8C);
    MARATHON_ASSERT_OFFSETOF(SaveDataTaskXENON, m_Operation, 0x90);
    MARATHON_ASSERT_SIZEOF(SaveDataTaskXENON, 0x98);
}
