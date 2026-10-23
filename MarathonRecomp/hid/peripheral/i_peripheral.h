#pragma once

#include <string>
#include <hid/peripheral/peripheral_type.h>
#include <hid/peripheral/peripheral_vendor.h>
#include <message_receiver.h>
#include <xbox.h>

namespace HID
{
    class IPeripheral : public IMessageReceiver
    {
    public:
        virtual ~IPeripheral() = default;

        virtual uint32_t GetID() const = 0;
        virtual std::string GetName() const = 0;
        virtual PeripheralType GetType() const = 0;
        virtual PeripheralVendor GetVendor() const = 0;

        virtual XAMINPUT_GAMEPAD GetState() = 0;

        virtual const XAMINPUT_VIBRATION& GetVibration() const = 0;
        virtual bool SetVibration(const XAMINPUT_VIBRATION& in_rVibration, const uint32_t in_duration = 5000) = 0;

        virtual bool IsMutator(const float in_deadzone = 0.12f) const = 0;
    };
}
