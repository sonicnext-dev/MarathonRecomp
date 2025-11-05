#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IDynamicLink
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IDynamicLink, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IDynamicLink, 4);
}
