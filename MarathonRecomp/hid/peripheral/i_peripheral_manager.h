#pragma once

#include <hid/peripheral/i_peripheral_observer.h>

namespace HID
{
    class IPeripheralManager : public IPeripheralObserver
    {
    public:
        virtual ~IPeripheralManager() = default;

        virtual bool Register(std::shared_ptr<IPeripheral> in_spPeripheral) = 0;
        virtual bool Unregister(IPeripheral* in_pPeripheral) = 0;
    };
}
