#pragma once

#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/System/CreateStatic.h>

namespace Sonicteam::System::Diagnostics
{
    class Perfomance : public Singleton<Perfomance, 0x82D3B210, CreateStatic<Perfomance, 0x82581C88>>
    {
    public:
        be<uint64_t> m_LastFrequency;
    };
}
