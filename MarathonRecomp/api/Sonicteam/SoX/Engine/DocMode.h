#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam::SoX::Engine
{
    class DocMode : public Task
    {
    public:
        be<uint32_t> m_Type;
    };

    MARATHON_ASSERT_OFFSETOF(DocMode, m_Type, 0x4C);
    MARATHON_ASSERT_SIZEOF(DocMode, 0x50);
}
