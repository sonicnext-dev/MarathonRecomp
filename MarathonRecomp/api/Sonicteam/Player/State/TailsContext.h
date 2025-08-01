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
    };
}
