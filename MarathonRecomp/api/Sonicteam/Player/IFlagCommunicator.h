#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IFlagCommunicator
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IFlagCommunicator, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IFlagCommunicator, 4);
}
