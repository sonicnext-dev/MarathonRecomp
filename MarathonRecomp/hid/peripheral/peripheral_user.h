#pragma once

#include <hid/peripheral/peripheral_observer.h>
#include <xbox.h>

namespace HID
{
    class PeripheralUser : public PeripheralObserver
    {
    private:
        XAMINPUT_GAMEPAD m_LastState{};
        XAMINPUT_VIBRATION m_LastVibration{};

    public:
        const uint32_t ID{};

        explicit PeripheralUser(const uint32_t in_id) : ID(in_id) {}

        virtual bool Attach(std::shared_ptr<IPeripheral> in_spPeripheral) override;
        virtual bool Detach(IPeripheral* in_pPeripheral) override;

        XAMINPUT_GAMEPAD GetState();

        XAMINPUT_VIBRATION GetVibration();
        bool SetVibration(const XAMINPUT_VIBRATION& in_rVibration, const uint32_t in_duration = 5000);

        bool IsFree() const;

        virtual bool OnBroadcastMessage(IMessage& io_rMessage) override;
        virtual bool OnProcessMessage(IMessage& io_rMessage) override;
    };
}
