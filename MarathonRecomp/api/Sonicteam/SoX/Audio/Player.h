#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Audio
{
    class Player
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(Player, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(Player, 4);
}
