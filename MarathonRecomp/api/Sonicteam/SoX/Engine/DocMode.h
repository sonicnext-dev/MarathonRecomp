#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam::SoX::Engine
{
    class DocMode : public Task
    {
    public:
        be<uint32_t> m_type;
    };
}
