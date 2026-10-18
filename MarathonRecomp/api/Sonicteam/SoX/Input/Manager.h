#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Input
{
    enum PadButton : uint32_t
    {
        PadButton_DPadUp = 0x40,
        PadButton_DPadDown = 0x80,
        PadButton_DPadLeft = 0x100,
        PadButton_DPadRight = 0x200,
        PadButton_Start = 0x400,
        PadButton_Back = 0x800,
        PadButton_LeftStick = 0x10000,
        PadButton_RightStick = 0x20000,
        PadButton_LeftShoulder = 0x1000,
        PadButton_RightShoulder = 0x2000,
        PadButton_South = 0x01,
        PadButton_East = 0x02,
        PadButton_West = 0x08,
        PadButton_North = 0x10,
        PadButton_LeftTrigger = 0x4000,
        PadButton_RightTrigger = 0x8000
    };

    struct PadState
    {
        be<PadButton> LastButtons;
        be<PadButton> InvertedLastButtons;
        be<PadButton> PressedButtons;
        be<PadButton> ReleasedButtons;
        be<float> LeftStickHorizontal;
        be<float> LeftStickVertical;
        be<short> LeftStickHorizontalS16;
        be<short> LeftStickVerticalS16;
        be<float> RightStickHorizontal;
        be<float> RightStickVertical;
        be<short> RightStickHorizontalS16;
        be<short> RightStickVerticalS16;

        bool IsDown(const PadButton buttons) const;
        bool IsPressed(const PadButton buttons) const;
        bool IsReleased(const PadButton buttons) const;
    };

    struct Manager
    {
        be<uint32_t> ID;
        MARATHON_INSERT_PADDING(0x0C);
        PadState PadState;
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
    MARATHON_ASSERT_SIZEOF(PadState, 0x28);

    MARATHON_ASSERT_OFFSETOF(Manager, ID, 0x00);
    MARATHON_ASSERT_OFFSETOF(Manager, PadState, 0x10);
    MARATHON_ASSERT_SIZEOF(Manager, 0x60);
}

#include "Manager.inl"
