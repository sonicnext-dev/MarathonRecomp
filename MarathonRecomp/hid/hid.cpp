#include "hid.h"
#include <ui/game_window.h>
#include <user/config.h>

namespace HID
{
    bool GetState(uint32_t in_userIndex, XAMINPUT_STATE* out_pState)
    {
        static uint32_t packetNumber{};

        if (!out_pState)
            return false;

        if (const auto pUser = GetManager()->GetUser(in_userIndex))
        {
            XAMINPUT_STATE state
            {
                .dwPacketNumber = packetNumber++,
                .Gamepad = pUser->GetState()
            };

            *out_pState = state;

            return true;
        }

        return false;
    }

    bool SetState(uint32_t in_userIndex, XAMINPUT_VIBRATION* in_pVibration)
    {
        if (!in_pVibration)
            return false;

        if (const auto pUser = GetManager()->GetUser(in_userIndex))
        {
            pUser->SetVibration(*in_pVibration);
            return true;
        }

        return false;
    }

    bool GetCapabilities(uint32_t in_userIndex, XAMINPUT_CAPABILITIES* out_pCapabilities)
    {
        if (!out_pCapabilities)
            return false;

        if (const auto pUser = GetManager()->GetUser(in_userIndex))
        {
            XAMINPUT_CAPABILITIES capabilities
            {
                .Type = XAMINPUT_DEVTYPE_GAMEPAD,
                .SubType = XAMINPUT_DEVSUBTYPE_GAMEPAD,
                .Flags = 0,
                .Gamepad = pUser->GetState(),
                .Vibration = pUser->GetVibration()
            };

            *out_pCapabilities = capabilities;

            return true;
        }

        return false;
    }

    bool IsInputAllowed()
    {
        return GameWindow::s_isFocused || Config::AllowBackgroundInput;
    }
}
