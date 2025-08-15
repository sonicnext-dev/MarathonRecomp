#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Input
{
    static const uint32_t XENON_GAMEPAD_BACK = 2048;
    struct MGamepad
    {
        be<uint32_t> wLastButtons;
        be<uint32_t> wCLastButtons; // ~wLastButtons
        MARATHON_INSERT_PADDING(8);

        //Left Stick
        be<float> fX1;
        be<float> fY1;
        be<short> sThumbLX;
        be<short> sThumbLY;

        //Right Stick
        be<float> fX2;
        be<float> fY2;
        be<short> sThumbRX;
        be<short> sThumbRY;
    };

    class Manager
    {
    public:
        be<uint32_t> m_ControllerID;
        MARATHON_INSERT_PADDING(0xC);
        MGamepad m_Gamepad;
        MARATHON_INSERT_PADDING(0x28);
    };
}
