#include "peripheral_observer.h"
#include <hid/message/messages.h>
#include <hid/hid.h>

namespace HID
{
    bool PeripheralObserver::Attach(std::shared_ptr<IPeripheral> in_spPeripheral)
    {
        if (!in_spPeripheral)
            return false;

        if (!GetManager()->Register(in_spPeripheral))
            return false;

        m_Observees.push_back(in_spPeripheral);

        in_spPeripheral->ProcessMessage<MsgPeripheralAttached>();

        return true;
    }

    bool PeripheralObserver::Detach(IPeripheral* in_pPeripheral)
    {
        if (!in_pPeripheral)
            return false;

        in_pPeripheral->ProcessMessage<MsgPeripheralDetached>();

        if (in_pPeripheral == m_LastMutator)
            m_LastMutator = nullptr;

        return std::erase_if(m_Observees, [&](std::weak_ptr<IPeripheral> in_wpPeripheral)
        {
            const auto spPeripheral = in_wpPeripheral.lock();

            if (!spPeripheral)
                return false;

            return spPeripheral->GetID() == in_pPeripheral->GetID();
        });
    }

    bool PeripheralObserver::Detach()
    {
        auto result = true;

        Walk([&](IPeripheral* in_pNode) -> bool
        {
            if (!Detach(in_pNode))
                result = false;

            return true;
        });

        return result;
    }

    IPeripheral* PeripheralObserver::GetMutator()
    {
        IPeripheral* result{};
        bool found{};

        Walk([&](IPeripheral* in_pNode) -> bool
        {
            if (in_pNode->IsMutator())
            {
                found = true;
                result = in_pNode;
                return false;
            }

            return true;
        });

        if (found)
            m_LastMutator = result;

        return m_LastMutator;
    }

    IPeripheral* PeripheralObserver::GetLastMutator()
    {
        return m_LastMutator;
    }

    IPeripheral* PeripheralObserver::Find(const uint32_t in_id)
    {
        IPeripheral* result{};

        Walk([&](IPeripheral* in_pNode) -> bool
        {
            if (in_pNode->GetID() == in_id)
            {
                result = in_pNode;
                return false;
            }

            return true;
        });

        return result;
    }

    bool PeripheralObserver::Walk(const PeripheralWalker& in_rVisitor)
    {
        for (const auto& wpPeripheral : m_Observees)
        {
            const auto spPeripheral = wpPeripheral.lock();

            if (!spPeripheral)
                continue;

            if (!in_rVisitor(spPeripheral.get()))
                break;
        }

        return false;
    }
}
