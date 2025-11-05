#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class FrustumCullingTask : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x30);
    };

    MARATHON_ASSERT_SIZEOF(FrustumCullingTask, 0x7C);
}
