#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class LockBlendMode : public MyRenderProcess
    {
    public:
        bool m_Lock;
    };

    MARATHON_ASSERT_OFFSETOF(LockBlendMode, m_Lock, 0x38);
    MARATHON_ASSERT_SIZEOF(LockBlendMode, 0x3C);
}
