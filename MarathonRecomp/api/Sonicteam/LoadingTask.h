#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/HUDLoading.h>

namespace Sonicteam
{
    class LoadingTask : public SoX::RefCountObject, public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x04);
        SoX::RefSharedPointer<HUDLoading> m_spHUDLoading;
    };

    MARATHON_ASSERT_OFFSETOF(LoadingTask, m_spHUDLoading, 0x58);
    MARATHON_ASSERT_SIZEOF(LoadingTask, 0x5C);
}
