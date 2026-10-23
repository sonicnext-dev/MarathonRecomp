#pragma once

#include <vector>
#include <hid/peripheral/i_peripheral_observer.h>
#include <hid/peripheral/i_peripheral.h>

namespace HID
{
    class PeripheralObserver : public IPeripheralObserver
    {
    protected:
        std::vector<std::weak_ptr<IPeripheral>> m_Observees{};
        IPeripheral* m_LastMutator{};

    public:
        virtual bool Attach(std::shared_ptr<IPeripheral> in_spPeripheral) override;
        virtual bool Detach(IPeripheral* in_pPeripheral) override;
        virtual bool Detach() override;

        virtual IPeripheral* GetMutator() override;
        virtual IPeripheral* GetLastMutator() override;

        virtual IPeripheral* Find(const uint32_t in_id) override;
        virtual bool Walk(const PeripheralWalker& in_rVisitor) override;
    };
}
