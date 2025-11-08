#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Thread.h>

namespace Sonicteam
{
    class NoSyncThread : SoX::Thread {};

    MARATHON_ASSERT_SIZEOF(NoSyncThread, 0x4C);
}
