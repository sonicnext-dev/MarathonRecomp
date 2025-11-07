#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetZMode : public MyRenderProcess
    {
    public:
        enum ZMode : uint32_t
        {
            LessEqualOn = 1,
            LessEqualOff = 2,
            LessOn = 3,
            LessOff = 4,
            AlwaysOn = 5,
            AlwaysOff = 6,
        };

        be<ZMode> m_ZMode;
    };

    MARATHON_ASSERT_SIZEOF(SetZMode, 0x3C);
    MARATHON_ASSERT_OFFSETOF(SetZMode, m_ZMode, 0x38);
}
