#pragma once

#include <hid/peripheral/i_peripheral_manager.h>
#include <hid/peripheral/i_peripheral.h>
#include <hid/peripheral/peripheral_user.h>

namespace HID
{
    class PeripheralManager : public IPeripheralManager
    {
    protected:
        IPeripheral* m_LastMutator{};

    public:
        virtual bool Init();

        virtual bool Attach(std::shared_ptr<IPeripheral> in_spPeripheral) override;
        virtual bool Detach(IPeripheral* in_pPeripheral) override;
        virtual bool Detach() override;

        virtual IPeripheral* GetMutator() override;
        virtual IPeripheral* GetLastMutator() override;

        virtual IPeripheral* Find(const uint32_t in_id) override;
        virtual bool Walk(const IPeripheralManager::PeripheralWalker& in_rVisitor) override;

        virtual bool Register(std::shared_ptr<IPeripheral> in_spPeripheral) override;
        virtual bool Unregister(IPeripheral* in_pPeripheral) override;

        virtual bool OnBroadcastMessage(IMessage& io_rMessage) override;
        virtual bool OnProcessMessage(IMessage& io_rMessage) override;

        PeripheralUser* GetUser(const uint32_t in_userId);
        std::vector<PeripheralUser*> FindUsers(const IPeripheral* in_pPeripheral);
    };
}
