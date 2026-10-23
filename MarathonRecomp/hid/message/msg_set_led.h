#pragma once

#include <Marathon.h>
#include <message.h>

namespace HID
{
    class MsgSetLED : public Message<MsgSetLED>
    {
    public:
        Sonicteam::SoX::RGBA8888 Color{};

        MsgSetLED(const uint8_t in_r, const uint8_t in_g, const uint8_t in_b, const uint8_t in_a = 255)
            : Color({ in_r, in_g, in_b, in_a }) {}

        MsgSetLED(const Sonicteam::SoX::RGBA8888& in_rColor)
            : Color(in_rColor) {}
    };
}
