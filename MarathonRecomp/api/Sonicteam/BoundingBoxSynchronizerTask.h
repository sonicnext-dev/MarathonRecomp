#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/Scenery/DrawableCallback.h>

namespace Sonicteam
{
    class BoundingBoxSynchronizerTask : public SoX::Engine::Task, public SoX::Scenery::DrawableCallback
    {
    public:
        MARATHON_INSERT_PADDING(0x8);
    };

    MARATHON_ASSERT_SIZEOF(BoundingBoxSynchronizerTask, 0x54);
}
