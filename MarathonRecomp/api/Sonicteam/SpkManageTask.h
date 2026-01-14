#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/Spanverse/AckManager.h>

namespace Sonicteam
{
    class SpkManageTask : public SoX::Engine::Task
    {
    public:
        xpointer<Spanverse::AckManager> m_pAckManager;
    };

    MARATHON_ASSERT_OFFSETOF(SpkManageTask, m_pAckManager, 0x4C);
    MARATHON_ASSERT_SIZEOF(SpkManageTask, 0x50);
}
