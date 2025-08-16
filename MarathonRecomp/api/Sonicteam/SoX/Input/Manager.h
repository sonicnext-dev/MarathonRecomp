#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Input
{
    static const uint32_t XENON_GAMEPAD_BACK = 2048;

    enum KeyState
    {
        KeyState_DpadUp = 0x40,
        KeyState_DpadDown = 0x80,
        KeyState_DpadLeft = 0x100,
        KeyState_DpadRight = 0x200,
        KeyState_Start = 0x400,
        KeyState_Select = 0x800,
        KeyState_LeftStick = 0x10000,
        KeyState_RightStick = 0x20000,
        KeyState_LeftBumper = 0x1000,
        KeyState_RightBumper = 0x2000,
        KeyState_A = 0x1,
        KeyState_B = 0x2,
        KeyState_X = 0x8,
        KeyState_Y = 0x10,
    };

    struct PadState
    {
        be<uint32_t> LastButtons;
        be<uint32_t> InvertedLastButtons; // ~wLastButtons
        MARATHON_INSERT_PADDING(8);

        be<float> LeftStickHorizontal;
        be<float> LeftStickVertical;
        be<short> LeftStickHorizontalS16;
        be<short> LeftStickVerticalS16;

        be<float> RightStickHorizontal;
        be<float> RightStickVertical;
        be<short> RightStickHorizontalS16;
        be<short> RightStickVerticalS16;
    };

    class Manager
    {
    public:
        be<uint32_t> m_ControllerID;
        MARATHON_INSERT_PADDING(0xC);
        PadState m_PadState;
        MARATHON_INSERT_PADDING(0x28);
    };
}
