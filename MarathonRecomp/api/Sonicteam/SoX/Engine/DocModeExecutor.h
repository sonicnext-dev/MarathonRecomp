#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam::SoX::Engine
{
    class DocModeExecutor : public Task {};

    MARATHON_ASSERT_SIZEOF(DocModeExecutor, 0x4C);
}
