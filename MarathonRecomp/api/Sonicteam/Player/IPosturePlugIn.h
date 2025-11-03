#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IPosturePlugIn
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IPosturePlugIn, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IPosturePlugIn, 4);
}
