#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/MessageReceiver.h>

namespace Sonicteam
{
    class PauseAdapter : public SoX::Engine::Task
    {
    public:
        xpointer<SoX::MessageReceiver> m_pOwner;
        be<uint32_t> m_SelectedID;
        be<uint32_t> m_Field54;
        MARATHON_INSERT_PADDING(0x20);
    };

    MARATHON_ASSERT_OFFSETOF(PauseAdapter, m_pOwner, 0x4C);
    MARATHON_ASSERT_OFFSETOF(PauseAdapter, m_SelectedID, 0x50);
    MARATHON_ASSERT_OFFSETOF(PauseAdapter, m_Field54, 0x54);
    MARATHON_ASSERT_SIZEOF(PauseAdapter, 0x78);
}
