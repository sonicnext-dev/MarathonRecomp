#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>
#include <api/stdx/string.h>

namespace Sonicteam
{
    class NamedTask : public SoX::Engine::Task
    {
    public:
        stdx::string m_Name;
    };
}
