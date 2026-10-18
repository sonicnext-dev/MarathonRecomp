#pragma once

#include <array>
#include <string>
#include <Marathon.h>
#include <message.h>
#include <xxHashMap.h>

namespace HID
{
    class MsgSetCharacterLED : public Message<MsgSetCharacterLED>
    {
    private:
        // TODO: tune these a little bit, some of them look the same.
        static inline xxHashMap<Sonicteam::SoX::RGB888> m_CharacterColours =
        {
            { HashStr("amy"),      { { 190, 105, 165 } } },
            { HashStr("blaze"),    { { 165, 120, 215 } } },
            { HashStr("knuckles"), { { 165, 32,  32  } } },
            { HashStr("omega"),    { { 210, 40,  25  } } },
            { HashStr("rouge"),    { { 85,  50,  115 } } },
            { HashStr("shadow"),   { { 255, 0,   0   } } },
            { HashStr("silver"),   { { 100, 175, 175 } } },
            { HashStr("sonic"),    { { 0,   0,   255 } } },
            { HashStr("tails"),    { { 255, 185, 52  } } }
        };

    public:
        Sonicteam::SoX::RGB888 Color{};

        MsgSetCharacterLED(const std::string_view& in_rCharacterName) : Color(m_CharacterColours[HashStr(in_rCharacterName)]) {}
    };
}
