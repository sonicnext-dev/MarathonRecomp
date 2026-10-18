#include "peripheral_manager.h"
#include <array>
#include <vector>
#include <hid/event/loading_led_event.h>
#include <hid/message/messages.h>
#include <os/logger.h>
#include <patches/loading_patches.h>

namespace HID
{
    static std::array<PeripheralUser, 4> g_users
    {
        PeripheralUser(0),
        PeripheralUser(1),
        PeripheralUser(2),
        PeripheralUser(3)
    };

    static std::vector<std::shared_ptr<IPeripheral>> g_peripherals{};

    bool PeripheralManager::Init()
    {
        // Register keyboard to first gamepad.
        ProcessMessage<MsgRegisterKeyboard>(0);

        static LoadingLEDEvent loadingLedEvent{};
        LoadingPatches::Events.push_back(&loadingLedEvent);

        return true;
    }

    bool PeripheralManager::Attach(std::shared_ptr<IPeripheral> in_spPeripheral)
    {
        if (!in_spPeripheral)
            return false;

        Register(in_spPeripheral);

        for (int i = 0; i < g_users.size(); i++)
        {
            if (!g_users[i].IsFree())
                continue;

            const auto peripheralName = in_spPeripheral->GetName();

            if (g_users[i].Attach(in_spPeripheral))
            {
                LOGFN("Peripheral attached for User {}: {}", i + 1, peripheralName);
                return true;
            }
            else
            {
                LOGFN_ERROR("Peripheral failed to attach for User {}: {}", i + 1, peripheralName);
                break;
            }
        }

        return false;
    }

    bool PeripheralManager::Detach(IPeripheral* in_pPeripheral)
    {
        bool result{};

        if (!in_pPeripheral)
            return result;

        result = true;

        for (int i = 0; i < g_users.size(); i++)
        {
            if (g_users[i].IsFree())
                continue;

            const auto peripheralName = in_pPeripheral->GetName();

            if (g_users[i].Detach(in_pPeripheral))
            {
                LOGFN("Peripheral detached for User {}: {}", i + 1, peripheralName);
            }
            else
            {
                LOGFN_ERROR("Peripheral failed to detach for User {}: {}", i + 1, peripheralName);
                result = false;
            }
        }

        return result;
    }

    bool PeripheralManager::Detach()
    {
        return false;
    }

    IPeripheral* PeripheralManager::GetMutator()
    {
        for (auto& rUser : g_users)
        {
            if (const auto pMutator = rUser.GetMutator())
                return pMutator;
        }

        return nullptr;
    }

    IPeripheral* PeripheralManager::Find(const uint32_t in_id)
    {
        for (const auto& spPeripheral : g_peripherals)
        {
            if (spPeripheral->GetID() == in_id)
                return spPeripheral.get();
        }

        return nullptr;
    }

    bool PeripheralManager::Walk(const IPeripheralManager::PeripheralWalker& in_rVisitor)
    {
        for (const auto& spPeripheral : g_peripherals)
        {
            if (!in_rVisitor(spPeripheral.get()))
                break;
        }

        return false;
    }

    bool PeripheralManager::Register(std::shared_ptr<IPeripheral> in_spPeripheral)
    {
        if (!in_spPeripheral)
            return false;

        bool found{};

        for (const auto& spPeripheral : g_peripherals)
        {
            if (spPeripheral->GetID() == in_spPeripheral->GetID())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            g_peripherals.push_back(in_spPeripheral);

            LOGFN("Peripheral registered: {}", in_spPeripheral->GetName());
        }

        return true;
    }

    bool PeripheralManager::Unregister(IPeripheral* in_pPeripheral)
    {
        const auto peripheralName = in_pPeripheral->GetName();

        const auto result = std::erase_if(g_peripherals, [&](std::shared_ptr<IPeripheral> in_spPeripheral)
        {
            return in_spPeripheral->GetID() == in_pPeripheral->GetID();
        });

        if (result)
            LOGFN("Peripheral unregistered: {}", peripheralName);

        return result;
    }

    bool PeripheralManager::OnBroadcastMessage(IMessage& io_rMessage)
    {
        auto result = true;

        for (auto& rUser : g_users)
        {
            if (!rUser.BroadcastMessage(io_rMessage))
                result = false;

            if (!rUser.ProcessMessage(io_rMessage))
                result = false;
        }

        return result;
    }

    bool PeripheralManager::OnProcessMessage(IMessage& io_rMessage)
    {
        if (io_rMessage.Is<MsgGetPeripheralUsers>())
        {
            const auto pMsgGetPeripheralUsers = static_cast<MsgGetPeripheralUsers*>(&io_rMessage);

            pMsgGetPeripheralUsers->Users = FindUsers(pMsgGetPeripheralUsers->pSender);
        }
        return true;
    }

    PeripheralUser* PeripheralManager::GetUser(const uint32_t in_userId)
    {
        if (in_userId >= g_users.size())
            return nullptr;

        return &g_users[in_userId];
    }

    std::vector<PeripheralUser*> PeripheralManager::FindUsers(const IPeripheral* in_pPeripheral)
    {
        std::vector<PeripheralUser*> result{};

        if (!in_pPeripheral)
            return result;

        for (auto& rUser : g_users)
        {
            rUser.Walk([&](IPeripheral* in_pNode) -> bool
            {
                if (in_pNode->GetID() == in_pPeripheral->GetID())
                {
                    result.push_back(&rUser);
                    return false;
                }

                return true;
            });
        }

        return result;
    }
}
