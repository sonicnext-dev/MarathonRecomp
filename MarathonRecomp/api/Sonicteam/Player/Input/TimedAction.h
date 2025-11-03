#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::Input
{
    struct TimedAction
    {
        be<float> m_Time;
        be<SoX::Input::KeyState> m_Action;
        bool m_Field08;
        bool m_IsDoubleTapped;
        bool m_Field0A;
        bool m_Field0B;
        be<float> m_Field0C;
    };

    MARATHON_ASSERT_OFFSETOF(TimedAction, m_Time, 0x00);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_Action, 0x04);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_Field08, 0x08);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_IsDoubleTapped, 0x09);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_Field0A, 0x0A);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_Field0B, 0x0B);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_Field0C, 0x0C);
}
