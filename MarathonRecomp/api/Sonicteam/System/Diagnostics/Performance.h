#pragma once

#include <Marathon.inl>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/System/CreateStatic.h>

namespace Sonicteam::System::Diagnostics
{
    class Performance : public Singleton<Performance, 0x82D3B210, CreateStatic<Performance, 0x82581C88>>
    {
    public:
        be<uint64_t> m_LastFrequency;
    };

    MARATHON_ASSERT_OFFSETOF(Performance, m_LastFrequency, 0x00);
    MARATHON_ASSERT_SIZEOF(Performance, 8);
}
