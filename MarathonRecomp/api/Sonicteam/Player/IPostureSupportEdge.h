#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IPostureSupportEdge
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x6C);
    };

    MARATHON_ASSERT_OFFSETOF(IPostureSupportEdge, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IPostureSupportEdge, 0x70);
}
