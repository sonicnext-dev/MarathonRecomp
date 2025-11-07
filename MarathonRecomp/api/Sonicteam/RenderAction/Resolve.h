#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class Resolve : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x8);
        xpointer<void> m_pFBO;
    };

    MARATHON_ASSERT_OFFSETOF(Resolve, m_pFBO, 0x40);
    MARATHON_ASSERT_SIZEOF(Resolve, 0x44);
}
