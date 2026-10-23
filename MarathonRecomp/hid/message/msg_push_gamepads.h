#pragma once

#include <message.h>

namespace HID
{
    class MsgPushGamepads : public Message<MsgPushGamepads>
    {
    public:
        uint32_t UserID{};

        MsgPushGamepads(const uint32_t in_userId) : UserID(in_userId) {}
    };
}
