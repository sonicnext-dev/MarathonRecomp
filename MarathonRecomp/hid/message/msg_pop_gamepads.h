#pragma once

#include <message.h>

namespace HID
{
    class MsgPopGamepads : public Message<MsgPopGamepads>
    {
    public:
        uint32_t UserID{};

        MsgPopGamepads(const uint32_t in_userId) : UserID(in_userId) {}
    };
}
