#pragma once

#include <functional>
#include <hid/peripheral/i_peripheral.h>
#include <message_receiver.h>

namespace HID
{
    class IPeripheralObserver : public IMessageReceiver
    {
    public:
        using PeripheralWalker = std::function<bool(IPeripheral*)>;

        virtual ~IPeripheralObserver() = default;

        virtual bool Attach(std::shared_ptr<IPeripheral> in_spPeripheral) = 0;
        virtual bool Detach(IPeripheral* in_pPeripheral) = 0;
        virtual bool Detach() = 0;

        virtual IPeripheral* GetMutator() = 0;
        virtual IPeripheral* GetLastMutator() = 0;

        virtual IPeripheral* Find(const uint32_t in_id) = 0;
        virtual bool Walk(const PeripheralWalker& in_rVisitor) = 0;
    };
}
