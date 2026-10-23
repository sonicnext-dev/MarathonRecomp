#pragma once

#include <string>
#include <Marathon.h>
#include <message.h>
#include <xxHashMap.h>

namespace HID
{
    class MsgSetCharacterLED : public Message<MsgSetCharacterLED>
    {
    private:
        static inline xxHashMap<Sonicteam::SoX::RGBA8888> m_CharacterColours =
        {
            { HashStr("amy"),      { { 255, 0,   170, 255 } } },
            { HashStr("blaze"),    { { 150, 0,   255, 255 } } },
            { HashStr("knuckles"), { { 213, 42,  37,  255 } } },
            { HashStr("omega"),    { { 255, 0,   0,   128 } } },
            { HashStr("rouge"),    { { 64,  0,   255, 204 } } },
            { HashStr("shadow"),   { { 255, 0,   0,   255 } } },
            { HashStr("silver"),   { { 17,  255, 255, 255 } } },
            { HashStr("sonic"),    { { 0,   21,  255, 255 } } },
            { HashStr("tails"),    { { 255, 255, 0,   255 } } }
        };

    public:
        Sonicteam::SoX::RGBA8888 Color{};

        MsgSetCharacterLED(const std::string_view& in_rCharacterName) : Color(m_CharacterColours[HashStr(in_rCharacterName)]) {}
    };
}
