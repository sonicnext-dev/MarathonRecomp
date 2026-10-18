#pragma once

#include <Marathon.h>
#include <message.h>

namespace HID
{
    class MsgSetLED : public Message<MsgSetLED>
    {
    public:
        Sonicteam::SoX::RGB888 Color{};

        MsgSetLED(const uint8_t in_r, const uint8_t in_g, const uint8_t in_b)
            : Color({ in_r, in_g, in_b }) {}

        MsgSetLED(const Sonicteam::SoX::RGB888& in_rColor)
            : Color(in_rColor) {}
    };
}
