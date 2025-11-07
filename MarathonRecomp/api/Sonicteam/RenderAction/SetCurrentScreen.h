#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetCurrentScreen : public MyRenderProcess
    {
    public:
        be<uint32_t> m_ScreenIndex;
    };

    MARATHON_ASSERT_SIZEOF(SetCurrentScreen, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetCurrentScreen, m_ScreenIndex, 0x38);
}
