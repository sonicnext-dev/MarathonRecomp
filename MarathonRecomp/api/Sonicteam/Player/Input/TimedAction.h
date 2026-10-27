#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Input/Manager.h>

namespace Sonicteam::Player::Input
{
    struct TimedAction
    {
        be<float> m_Time{};
        be<uint32_t> m_Button{};
        uint8_t m_Field08{};
        bool m_IsDoublePressed{};
        bool m_IsActive{};
        bool m_IsDown{};
        be<float> m_Duration{};

        TimedAction() = default;

        TimedAction(SoX::Input::PadButton in_button, float in_duration = 0.0f)
            : m_Button(in_button), m_Duration(in_duration) {}
    };

    MARATHON_ASSERT_OFFSETOF(TimedAction, m_Time, 0x00);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_Button, 0x04);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_Field08, 0x08);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_IsDoublePressed, 0x09);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_IsActive, 0x0A);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_IsDown, 0x0B);
    MARATHON_ASSERT_OFFSETOF(TimedAction, m_Duration, 0x0C);
    MARATHON_ASSERT_SIZEOF(TimedAction, 0x10);
}
