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
            SetZMode_LessEqualOn = 1,
            SetZMode_LessEqualOff = 2,
            SetZMode_LessOn = 3,
            SetZMode_LessOff = 4,
            SetZMode_AlwaysOn = 5,
            SetZMode_AlwaysOff = 6,
        };

        be<ZMode> m_ZMode;
    };

    MARATHON_ASSERT_OFFSETOF(SetZMode, m_ZMode, 0x38);
    MARATHON_ASSERT_SIZEOF(SetZMode, 0x3C);
}
