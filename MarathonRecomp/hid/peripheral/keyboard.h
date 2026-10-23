#pragma once

#include <hid/peripheral/peripheral.h>

namespace HID
{
    class Keyboard : public Peripheral
    {
    public:
        HID_MAKE_PERIPHERAL_ID(1);

        virtual std::string GetName() const override
        {
            return "Keyboard";
        }

        virtual PeripheralType GetType() const override
        {
            return PeripheralType::Keyboard;
        }
    };
}
