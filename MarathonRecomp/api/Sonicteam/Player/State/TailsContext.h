#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::State
{
    class TailsContext : public CommonContext
    {
    public:
        be<float> m_FlightTime;
        MARATHON_INSERT_PADDING(8);
        be<float> m_FlightDuration;
        be<float> m_FlightLimit;
        MARATHON_INSERT_PADDING(8);
    };

    MARATHON_ASSERT_OFFSETOF(TailsContext, m_FlightTime, 0x230);
    MARATHON_ASSERT_OFFSETOF(TailsContext, m_FlightDuration, 0x23C);
    MARATHON_ASSERT_OFFSETOF(TailsContext, m_FlightLimit, 0x240);
    MARATHON_ASSERT_SIZEOF(TailsContext, 0x250);
}
