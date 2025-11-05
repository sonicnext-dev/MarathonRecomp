#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IPostureSupportOttoto
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x6C);
    };

    MARATHON_ASSERT_OFFSETOF(IPostureSupportOttoto, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IPostureSupportOttoto, 0x70);
}
