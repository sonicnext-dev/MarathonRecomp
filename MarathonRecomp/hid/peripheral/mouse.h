#pragma once

#include <hid/peripheral/peripheral.h>

namespace HID
{
    class Mouse : public Peripheral
    {
    public:
        HID_MAKE_PERIPHERAL_ID(2);

        virtual std::string GetName() const override
        {
            return "Mouse";
        }

        virtual PeripheralType GetType() const override
        {
            return PeripheralType::Mouse;
        }
    };
}
