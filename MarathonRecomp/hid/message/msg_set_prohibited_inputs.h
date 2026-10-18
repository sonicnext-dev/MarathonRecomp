#pragma once

#include <message.h>

namespace HID
{
    class MsgSetProhibitedInputs : public Message<MsgSetProhibitedInputs>
    {
    public:
        uint16_t Buttons{};
        bool LeftStick{};
        bool RightStick{};

        MsgSetProhibitedInputs(const uint16_t in_buttons, const bool in_leftStick, const bool in_rightStick)
            : Buttons(in_buttons), LeftStick(in_leftStick), RightStick(in_rightStick) {}
    };
}
