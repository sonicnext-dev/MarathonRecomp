#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Input
{
    enum KeyState
    {
        KeyState_DPadUp = 0x40,
        KeyState_DPadDown = 0x80,
        KeyState_DPadLeft = 0x100,
        KeyState_DPadRight = 0x200,
        KeyState_Start = 0x400,
        KeyState_Select = 0x800,
        KeyState_LeftStick = 0x10000,
        KeyState_RightStick = 0x20000,
        KeyState_LeftBumper = 0x1000,
        KeyState_RightBumper = 0x2000,
        KeyState_A = 0x01,
        KeyState_B = 0x02,
        KeyState_X = 0x08,
        KeyState_Y = 0x10,
        KeyState_LeftTrigger = 0x4000,
        KeyState_RightTrigger = 0x8000,
    };

    struct PadState
    {
        be<uint32_t> LastButtons;
        be<uint32_t> InvertedLastButtons;
        be<uint32_t> PressedButtons;
        be<uint32_t> ReleasedButtons;
        be<float> LeftStickHorizontal;
        be<float> LeftStickVertical;
        be<short> LeftStickHorizontalS16;
        be<short> LeftStickVerticalS16;
        be<float> RightStickHorizontal;
        be<float> RightStickVertical;
        be<short> RightStickHorizontalS16;
        be<short> RightStickVerticalS16;

        bool IsDown(const KeyState in_Keys) const;
        bool IsPressed(const KeyState in_Keys) const;
        bool IsReleased(const KeyState in_Keys) const;
    };

    class Manager
    {
    public:
        be<uint32_t> m_PadID;
        MARATHON_INSERT_PADDING(0x0C);
        PadState m_PadState;
        MARATHON_INSERT_PADDING(0x28);
    };

    MARATHON_ASSERT_OFFSETOF(PadState, LastButtons, 0x00);
    MARATHON_ASSERT_OFFSETOF(PadState, InvertedLastButtons, 0x04);
    MARATHON_ASSERT_OFFSETOF(PadState, PressedButtons, 0x08);
    MARATHON_ASSERT_OFFSETOF(PadState, ReleasedButtons, 0x0C);
    MARATHON_ASSERT_OFFSETOF(PadState, LeftStickHorizontal, 0x10);
    MARATHON_ASSERT_OFFSETOF(PadState, LeftStickVertical, 0x14);
    MARATHON_ASSERT_OFFSETOF(PadState, LeftStickHorizontalS16, 0x18);
    MARATHON_ASSERT_OFFSETOF(PadState, LeftStickVerticalS16, 0x1A);
    MARATHON_ASSERT_OFFSETOF(PadState, RightStickHorizontal, 0x1C);
    MARATHON_ASSERT_OFFSETOF(PadState, RightStickVertical, 0x20);
    MARATHON_ASSERT_OFFSETOF(PadState, RightStickHorizontalS16, 0x24);
    MARATHON_ASSERT_OFFSETOF(PadState, RightStickVerticalS16, 0x26);

    MARATHON_ASSERT_OFFSETOF(Manager, m_PadID, 0x00);
    MARATHON_ASSERT_OFFSETOF(Manager, m_PadState, 0x10);
}

#include "Manager.inl"
