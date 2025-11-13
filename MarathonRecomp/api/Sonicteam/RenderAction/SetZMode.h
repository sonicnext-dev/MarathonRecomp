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
            ZMode_LessEqualOn = 1,
            ZMode_LessEqualOff = 2,
            ZMode_LessOn = 3,
            ZMode_LessOff = 4,
            ZMode_AlwaysOn = 5,
            ZMode_AlwaysOff = 6,
        };

        be<ZMode> m_ZMode;
    };

    MARATHON_ASSERT_OFFSETOF(SetZMode, m_ZMode, 0x38);
    MARATHON_ASSERT_SIZEOF(SetZMode, 0x3C);
}
