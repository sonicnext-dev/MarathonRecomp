#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/HUDLoading.h>

namespace Sonicteam
{
    // TODO (Hyper): revise guest vftable inheritance. SoX::RefCountObject is
    // supposed to override the vftable at offset 0 here, but it adds its own
    // vftable pointer, which is wrong.
    class LoadingTask : /* public SoX::RefCountObject, */ public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x0C);
        SoX::RefSharedPointer<HUDLoading> m_spHUDLoading;
    };

    MARATHON_ASSERT_OFFSETOF(LoadingTask, m_spHUDLoading, 0x58);
    MARATHON_ASSERT_SIZEOF(LoadingTask, 0x5C);
}
