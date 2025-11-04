#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetZMode : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(SetZMode, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetZMode, m_pMyGraphicsDevice, 0x34);
}
