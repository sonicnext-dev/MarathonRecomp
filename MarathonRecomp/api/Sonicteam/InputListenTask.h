#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/Input/Manager.h>
#include <stdx/vector.h>

namespace Sonicteam
{
    class InputListenTask : public SoX::Engine::Task
    {
    public:
        stdx::vector<xpointer<SoX::Input::Manager>> m_InputManagers;
    };

    MARATHON_ASSERT_OFFSETOF(InputListenTask, m_InputManagers, 0x4C);
    MARATHON_ASSERT_SIZEOF(InputListenTask, 0x5C);
}
