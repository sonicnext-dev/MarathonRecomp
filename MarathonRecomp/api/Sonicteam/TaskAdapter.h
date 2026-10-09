#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/MessageReceiver.h>

namespace Sonicteam
{
    class TaskAdapter : public SoX::Engine::Task
    {
    public:
        xpointer<SoX::MessageReceiver> m_pOwner;
    };

    MARATHON_ASSERT_OFFSETOF(TaskAdapter, m_pOwner, 0x4C);
    MARATHON_ASSERT_SIZEOF(TaskAdapter, 0x50);
}
