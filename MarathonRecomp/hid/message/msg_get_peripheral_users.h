#pragma once

#include <vector>
#include <hid/peripheral/peripheral_user.h>
#include <message.h>

namespace HID
{
    class MsgGetPeripheralUsers : public Message<MsgGetPeripheralUsers>
    {
    public:
        const IPeripheral* pSender{};
        std::vector<PeripheralUser*> Users{};

        MsgGetPeripheralUsers(const IPeripheral* in_pSender) : pSender(in_pSender) {}
    };
}
