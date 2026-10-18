#pragma once

#include <hid/peripheral/peripheral.h>

namespace HID
{
    class Gamepad : public Peripheral
    {
    protected:
        uint32_t m_ID{};
        void* m_pContext{};

    public:
        virtual uint32_t GetID() const override
        {
            return m_ID;
        }

        virtual PeripheralType GetType() const override
        {
            return PeripheralType::Gamepad;
        }

        virtual std::string GetName() const override
        {
            return "Gamepad";
        }

        template <typename T>
        T* GetContext() const
        {
            return static_cast<T*>(m_pContext);
        }
    };
}
