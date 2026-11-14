#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/CommonObject.h>

namespace Sonicteam::Player::State
{
    class CommonAmigo : public CommonObject
    {
    public:
        be<float> m_Time;
    };

    MARATHON_ASSERT_OFFSETOF(CommonAmigo, m_Time, 0x0C);
    MARATHON_ASSERT_SIZEOF(CommonAmigo, 0x10);
}
