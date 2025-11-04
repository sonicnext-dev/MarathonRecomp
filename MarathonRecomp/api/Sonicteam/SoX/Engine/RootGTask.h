#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/GTask.h>

namespace Sonicteam::SoX::Engine
{
    class RootGTask : public GTask {};

    MARATHON_ASSERT_SIZEOF(RootGTask, 0x34);
}
