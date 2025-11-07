#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class LockCullMode : public MyRenderProcess
    {
    public:
        bool m_Lock;
    };

    MARATHON_ASSERT_SIZEOF(LockCullMode, 0x3C);
    MARATHON_ASSERT_OFFSETOF(LockCullMode, m_Lock, 0x38);
}
