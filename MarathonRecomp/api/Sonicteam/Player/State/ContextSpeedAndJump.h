#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::State
{
    class ContextSpeedAndJump 
    {
    public:
        be<float> m_BaseSpeedForward;
        be<float> m_GimmickSpeedForward;
        be<float> m_BaseSpeedVertical;
        be<float> m_GimmickSpeedVertical;
    };

    MARATHON_ASSERT_OFFSETOF(ContextSpeedAndJump, m_BaseSpeedForward, 0x00);
    MARATHON_ASSERT_OFFSETOF(ContextSpeedAndJump, m_GimmickSpeedForward, 0x04);
    MARATHON_ASSERT_OFFSETOF(ContextSpeedAndJump, m_BaseSpeedVertical, 0x08);
    MARATHON_ASSERT_OFFSETOF(ContextSpeedAndJump, m_GimmickSpeedVertical, 0x0C);
    MARATHON_ASSERT_SIZEOF(ContextSpeedAndJump, 0x10);
}
