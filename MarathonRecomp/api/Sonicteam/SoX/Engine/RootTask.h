#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam::SoX::Engine
{
    class RootTask : public Task {};

    MARATHON_ASSERT_SIZEOF(RootTask, 0x4C);
}
