#include "peripheral_manager.h"
#include <array>
#include <vector>
#include <hid/event/loading_led_event.h>
#include <hid/message/messages.h>
#include <hid/peripheral/keyboard.h>
#include <hid/peripheral/mouse.h>
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
        // Register keyboard to first user.
        ProcessMessage<MsgRegisterKeyboard>(0);

        static LoadingLEDEvent loadingLedEvent{};
        LoadingPatches::Events.push_back(&loadingLedEvent);

        return true;
    }

    bool PeripheralManager::Attach(std::shared_ptr<IPeripheral> in_spPeripheral)
    {
        bool result{};

        if (!in_spPeripheral)
            return result;

        const auto keyboardUsers = FindUsers(Find(Keyboard::ID));

        // Unregister keyboard, we'll set it back up later.
        ProcessMessage<MsgUnregisterKeyboard>();

        for (int i = 0; i < g_users.size(); i++)
        {
            if (!g_users[i].IsFree())
                continue;

            if (g_users[i].Attach(std::move(in_spPeripheral)))
                result = true;

            break;
        }

        for (const auto& rUser : keyboardUsers)
        {
            // Only register one keyboard.
            ProcessMessage<MsgRegisterKeyboard>(rUser->ID);
            break;
        }

        return result;
    }

    bool PeripheralManager::Detach(IPeripheral* in_pPeripheral)
    {
        if (!in_pPeripheral)
            return false;

        for (int i = 0; i < g_users.size(); i++)
        {
            if (g_users[i].IsFree())
                continue;

            g_users[i].Detach(in_pPeripheral);
        }

        return true;
    }

    bool PeripheralManager::Detach()
    {
        return false;
    }

    IPeripheral* PeripheralManager::GetMutator()
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

    IPeripheral* PeripheralManager::GetLastMutator()
    {
        return m_LastMutator;
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

        for (auto& spPeripheral : g_peripherals)
        {
            if (spPeripheral->GetID() == in_spPeripheral->GetID())
            {
                LOGFN("Peripheral updated: {}", in_spPeripheral->GetName());

                spPeripheral = std::move(in_spPeripheral);

                return true;
            }
        }

        LOGFN("Peripheral registered: {}", in_spPeripheral->GetName());

        g_peripherals.push_back(std::move(in_spPeripheral));

        return true;
    }

    bool PeripheralManager::Unregister(IPeripheral* in_pPeripheral)
    {
        if (!in_pPeripheral)
            return false;

        const auto peripheralName = in_pPeripheral->GetName();

        const auto count = std::erase_if(g_peripherals, [&](std::shared_ptr<IPeripheral> in_spPeripheral)
        {
            return in_spPeripheral->GetID() == in_pPeripheral->GetID();
        });

        if (count)
            LOGFN("Peripheral unregistered: {}", peripheralName);

        return true;
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
        else if (io_rMessage.Is<MsgPopGamepads>() || io_rMessage.Is<MsgPushGamepads>())
        {
            const auto isPop = io_rMessage.Is<MsgPopGamepads>();

            const auto userId = isPop
                ? static_cast<MsgPopGamepads*>(&io_rMessage)->UserID
                : static_cast<MsgPushGamepads*>(&io_rMessage)->UserID;

            auto currentIds = std::vector<uint32_t>();
            auto nextIds = std::vector<uint32_t>();

            const auto start = isPop ? (g_users.size() - 1) : userId;

            // Pushes or pops gamepads in either direction, starting or stopping at the specified user ID.
            for (int i = start; isPop ? (i >= userId && i >= 0) : (i < g_users.size()); isPop ? i-- : i++)
            {
                auto& rUser = g_users[i];

                nextIds.swap(currentIds);
                currentIds.clear();

                // Gather all gamepad IDs for current user.
                rUser.Walk([&](IPeripheral* in_pNode) -> bool
                {
                    if (in_pNode->GetType() != PeripheralType::Gamepad)
                        return true;

                    currentIds.push_back(in_pNode->GetID());

                    return true;
                });

                // Detach all gamepads from current user.
                for (const auto& rId : currentIds)
                    rUser.Detach(rUser.Find(rId));

                if (nextIds.empty())
                    continue;

                // Attach all gamepads from previous user to this one.
                for (const auto& spPeripheral : g_peripherals)
                {
                    if (std::find(nextIds.begin(), nextIds.end(), spPeripheral->GetID()) == nextIds.end())
                        continue;

                    rUser.Attach(spPeripheral);
                }
            }
        }
        else if (io_rMessage.Is<MsgUnregisterKeyboard>())
        {
            Detach(Find(Keyboard::ID));
            Detach(Find(Mouse::ID));
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
