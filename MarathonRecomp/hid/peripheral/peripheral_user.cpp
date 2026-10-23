#include "peripheral_user.h"
#include <hid/message/messages.h>
#include <hid/peripheral/keyboard.h>
#include <hid/hid.h>
#include <os/logger.h>

namespace HID
{
    bool PeripheralUser::Attach(std::shared_ptr<IPeripheral> in_spPeripheral)
    {
        const auto peripheralId = in_spPeripheral->GetID();
        const auto peripheralType = in_spPeripheral->GetType();
        const auto peripheralName = in_spPeripheral->GetName();

        if (PeripheralObserver::Attach(std::move(in_spPeripheral)))
        {
            if (peripheralType == PeripheralType::Gamepad)
                GetManager()->ProcessMessage<MsgSetGamepadUser>(ID, peripheralId);

            LOGFN("Peripheral attached for User {}: {}", ID + 1, peripheralName);

            return true;
        }

        LOGFN_ERROR("Peripheral failed to attach for User {}: {}", ID + 1, peripheralName);

        return false;
    }

    bool PeripheralUser::Detach(IPeripheral* in_pPeripheral)
    {
        const auto peripheralName = in_pPeripheral->GetName();

        if (PeripheralObserver::Detach(in_pPeripheral))
        {
            LOGFN("Peripheral detached for User {}: {}", ID + 1, peripheralName);
            return true;
        }

        return false;
    }

    XAMINPUT_GAMEPAD PeripheralUser::GetState()
    {
        XAMINPUT_GAMEPAD result{};

        Walk([&](IPeripheral* in_pNode) -> bool
        {
            const auto& rState = in_pNode->GetState();

            result.wButtons |= rState.wButtons;

            result.bLeftTrigger = std::max(result.bLeftTrigger, rState.bLeftTrigger);
            result.bRightTrigger = std::max(result.bRightTrigger, rState.bRightTrigger);

            const auto absmax = [&](const auto& in_left, const auto& in_right)
            {
                return std::abs(in_left) > std::abs(in_right)
                    ? in_left
                    : in_right;
            };

            result.sThumbLX = absmax(result.sThumbLX, rState.sThumbLX);
            result.sThumbLY = absmax(result.sThumbLY, rState.sThumbLY);

            result.sThumbRX = absmax(result.sThumbRX, rState.sThumbRX);
            result.sThumbRY = absmax(result.sThumbRY, rState.sThumbRY);

            return true;
        });

        m_LastState = result;

        return result;
    }

    XAMINPUT_VIBRATION PeripheralUser::GetVibration()
    {
        XAMINPUT_VIBRATION result{};

        Walk([&](IPeripheral* in_pNode) -> bool
        {
            const auto& rVibration = in_pNode->GetVibration();

            result.wLeftMotorSpeed = std::max(result.wLeftMotorSpeed, rVibration.wLeftMotorSpeed);
            result.wRightMotorSpeed = std::max(result.wRightMotorSpeed, rVibration.wRightMotorSpeed);

            return true;
        });

        return result;
    }

    bool PeripheralUser::SetVibration(const XAMINPUT_VIBRATION& in_rVibration, const uint32_t in_duration)
    {
        auto result = true;

        Walk([&](IPeripheral* in_pNode) -> bool
        {
            if (!in_pNode->SetVibration(in_rVibration, in_duration))
                result = false;

            return true;
        });

        m_LastVibration = GetVibration();

        return result;
    }

    bool PeripheralUser::IsFree() const
    {
        return m_Observees.empty();
    }

    bool PeripheralUser::OnBroadcastMessage(IMessage& io_rMessage)
    {
        auto result = true;

        Walk([&](IPeripheral* in_pNode) -> bool
        {
            if (!in_pNode->ProcessMessage(io_rMessage))
                result = false;

            return true;
        });

        return result;
    }

    bool PeripheralUser::OnProcessMessage(IMessage& io_rMessage)
    {
        return true;
    }
}
