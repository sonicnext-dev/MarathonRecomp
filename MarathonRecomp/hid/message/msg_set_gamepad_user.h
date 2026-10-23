#pragma once

#include <message.h>

namespace HID
{
    class MsgSetGamepadUser : public Message<MsgSetGamepadUser>
    {
    public:
        uint32_t UserID{};
        uint32_t PeripheralID{};

        MsgSetGamepadUser(const uint32_t in_userId, const uint32_t in_peripheralId)
            : UserID(in_userId), PeripheralID(in_peripheralId) {}
    };
}
