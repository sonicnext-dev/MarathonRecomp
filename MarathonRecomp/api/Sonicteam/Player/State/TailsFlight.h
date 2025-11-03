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

    MARATHON_ASSERT_OFFSETOF(TailsFlight, m_FlightTime, 0x18);
    MARATHON_ASSERT_SIZEOF(TailsFlight, 0x1C);
}
