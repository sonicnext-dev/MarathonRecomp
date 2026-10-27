#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Input/Manager.h>

namespace Sonicteam::Player::Input
{
    struct AccumulativeAction
    {
        be<uint32_t> m_Button{};
        bool m_IsDoublePressed{};
        bool m_IsPressed{};
        bool m_IsReleased{};
        uint8_t m_Field07{};
        be<float> m_Time{};
        be<float> m_Duration{};
        bool m_IsActive{};

        AccumulativeAction() = default;

        AccumulativeAction(SoX::Input::PadButton in_button)
            : m_Button(in_button) {}
    };

    MARATHON_ASSERT_OFFSETOF(AccumulativeAction, m_Button, 0x00);
    MARATHON_ASSERT_OFFSETOF(AccumulativeAction, m_IsDoublePressed, 0x04);
    MARATHON_ASSERT_OFFSETOF(AccumulativeAction, m_IsPressed, 0x05);
    MARATHON_ASSERT_OFFSETOF(AccumulativeAction, m_IsReleased, 0x06);
    MARATHON_ASSERT_OFFSETOF(AccumulativeAction, m_Field07, 0x07);
    MARATHON_ASSERT_OFFSETOF(AccumulativeAction, m_Time, 0x08);
    MARATHON_ASSERT_OFFSETOF(AccumulativeAction, m_Duration, 0x0C);
    MARATHON_ASSERT_OFFSETOF(AccumulativeAction, m_IsActive, 0x10);
    MARATHON_ASSERT_SIZEOF(AccumulativeAction, 0x14);
}
