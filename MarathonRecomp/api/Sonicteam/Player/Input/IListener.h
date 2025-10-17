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
}
