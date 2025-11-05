#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class INotification
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(INotification, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(INotification, 4);
}
