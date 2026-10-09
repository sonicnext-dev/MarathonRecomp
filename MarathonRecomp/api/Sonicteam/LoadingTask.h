#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/HUDLoading.h>

namespace Sonicteam
{
    class LoadingTask : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<HUDLoading> m_pHUDLoading;
    };

    MARATHON_ASSERT_OFFSETOF(LoadingTask, m_pHUDLoading, 0x58);
    MARATHON_ASSERT_SIZEOF(LoadingTask, 0x5C);
}
