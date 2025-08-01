#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class Actor : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(0x0C);
    };
}
