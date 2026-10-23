#pragma once

#include <message.h>

namespace HID
{
    class MsgSetKeyboardUser : public Message<MsgSetKeyboardUser>
    {
    public:
        uint32_t UserID{};

        MsgSetKeyboardUser(const uint32_t in_userId) : UserID(in_userId) {}
    };
}
