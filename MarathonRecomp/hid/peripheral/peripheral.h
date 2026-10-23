#pragma once

#include <hid/peripheral/i_peripheral.h>

#define HID_MAKE_PERIPHERAL_ID(X)             \
    static inline constexpr uint32_t ID = -X; \
    virtual uint32_t GetID() const override { return ID; }

namespace HID
{
    class Peripheral : public IPeripheral
    {
    protected:
        XAMINPUT_GAMEPAD m_State{};
        XAMINPUT_GAMEPAD m_LastState{};
        XAMINPUT_VIBRATION m_LastVibration{};

    public:
        HID_MAKE_PERIPHERAL_ID(0);

        virtual std::string GetName() const override
        {
            return "Peripheral";
        }

        virtual PeripheralType GetType() const override
        {
            return PeripheralType::Virtual;
        }

        virtual PeripheralVendor GetVendor() const override
        {
            return PeripheralVendor::Xbox;
        }

        virtual XAMINPUT_GAMEPAD GetState() override
        {
            return m_State = {};
        }

        virtual const XAMINPUT_VIBRATION& GetVibration() const override
        {
            return m_LastVibration;
        }

        virtual bool SetVibration(const XAMINPUT_VIBRATION& in_rVibration, const uint32_t in_duration = 5000) override
        {
            m_LastVibration = in_rVibration;

            return true;
        }

        virtual bool IsMutator(const float in_deadzone = 0.12f) const override
        {
            return m_State.wButtons != 0 ||
                   m_State.bLeftTrigger != 0 ||
                   m_State.bRightTrigger != 0 ||
                   float(std::abs(m_State.sThumbLX)) / 65535.0f >= in_deadzone ||
                   float(std::abs(m_State.sThumbLY)) / 65535.0f >= in_deadzone ||
                   float(std::abs(m_State.sThumbRX)) / 65535.0f >= in_deadzone ||
                   float(std::abs(m_State.sThumbRY)) / 65535.0f >= in_deadzone;
        }

        virtual bool OnBroadcastMessage(IMessage& io_rMessage) override
        {
            return true;
        }

        virtual bool OnProcessMessage(IMessage& io_rMessage) override
        {
            return true;
        }
    };
}
