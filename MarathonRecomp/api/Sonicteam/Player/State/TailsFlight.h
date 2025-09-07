#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::State
{
    class TailsFlight : public CommonFall
    {
    public:
        MARATHON_INSERT_PADDING(4);
        be<float> m_FlightTime;
    };
}
