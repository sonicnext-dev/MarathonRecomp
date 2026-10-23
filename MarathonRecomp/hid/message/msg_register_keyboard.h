#pragma once

#include <message.h>

namespace HID
{
    class MsgRegisterKeyboard : public Message<MsgRegisterKeyboard>
    {
    public:
        uint32_t UserID{};

        MsgRegisterKeyboard(const uint32_t in_userId) : UserID(in_userId) {}
    };
}
