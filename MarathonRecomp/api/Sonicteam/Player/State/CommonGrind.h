#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/CommonObject.h>

namespace Sonicteam::Player::State
{
    class CommonGrind : public CommonObject
    {
    public:
        be<float> m_GrindTime;
        be<float> m_PenaltyTime;
    };

    MARATHON_ASSERT_OFFSETOF(CommonGrind, m_GrindTime, 0xC);
    MARATHON_ASSERT_OFFSETOF(CommonGrind, m_PenaltyTime, 0x10);
    MARATHON_ASSERT_SIZEOF(CommonGrind, 0x14);
}
