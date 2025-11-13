#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class LockZMode : public MyRenderProcess
    {
    public:
        bool m_Lock;
    };

    MARATHON_ASSERT_OFFSETOF(LockZMode, m_Lock, 0x38);
    MARATHON_ASSERT_SIZEOF(LockZMode, 0x3C);
}
