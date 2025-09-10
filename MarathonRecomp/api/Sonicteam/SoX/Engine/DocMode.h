#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Engine
{
    class DocMode : public Task
    {
    public:
        be<uint32_t> m_type;
    };
}
