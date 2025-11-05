#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IPostureSupportSphere
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0xCC);
    };

    MARATHON_ASSERT_OFFSETOF(IPostureSupportSphere, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IPostureSupportSphere, 0xD0);
}
