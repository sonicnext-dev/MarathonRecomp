#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Thread.h>

namespace Sonicteam
{
    class NoSyncThread : SoX::Thread
    {
    public:
        MARATHON_INSERT_PADDING(4);
    };

}
