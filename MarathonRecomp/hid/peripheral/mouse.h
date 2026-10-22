#pragma once

#include <hid/peripheral/peripheral.h>

namespace HID
{
    class Mouse : public Peripheral
    {
    public:
        float X{};
        float Y{};
        float RelativeX{};
        float RelativeY{};

        HID_MAKE_PERIPHERAL_ID(2);

        virtual std::string GetName() const override
        {
            return "Mouse";
        }

        virtual PeripheralType GetType() const override
        {
            return PeripheralType::Mouse;
        }

        virtual bool IsMutator(const float in_deadzone = 0.12f) const override
        {
            return Peripheral::IsMutator(in_deadzone) ||
                   std::fabsf(RelativeX) >= in_deadzone ||
                   std::fabsf(RelativeY) >= in_deadzone;
        }

        virtual void Accumulate(float* out_x, float* out_y) const
        {
            if (out_x)
                *out_x += RelativeX;

            if (out_y)
                *out_y += RelativeY;
        }
    };
}
