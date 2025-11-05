#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::Input
{
    class IListener
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x14);
    };

    MARATHON_ASSERT_OFFSETOF(IListener, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IListener, 0x18);
}
