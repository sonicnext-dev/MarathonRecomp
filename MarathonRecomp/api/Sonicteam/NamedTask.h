#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <stdx/string.h>

namespace Sonicteam
{
    class NamedTask : public SoX::Engine::Task
    {
    public:
        stdx::string m_Name;
    };

    MARATHON_ASSERT_OFFSETOF(NamedTask, m_Name, 0x4C);
    MARATHON_ASSERT_SIZEOF(NamedTask, 0x68);
}
