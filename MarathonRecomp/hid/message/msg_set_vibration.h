#pragma once

#include <message.h>
#include <xbox.h>

namespace HID
{
    class MsgSetVibration : public Message<MsgSetVibration>
    {
    public:
        XAMINPUT_VIBRATION Vibration{};
        uint32_t Duration{};

        MsgSetVibration(const XAMINPUT_VIBRATION& in_rVibration, const uint32_t in_duration = 5000)
            : Vibration(in_rVibration), Duration(in_duration) {}
    };
}
