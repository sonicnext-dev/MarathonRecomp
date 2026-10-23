#pragma once

#include <hid/driver/sdl/peripheral/keyboard_sdl.h>
#include <hid/message/messages.h>
#include <hid/peripheral/gamepad.h>
#include <hid/hid.h>
#include <SDL3/SDL.h>
#include <app.h>

namespace HID
{
    class GamepadSDL : public Gamepad
    {
    public:
        GamepadSDL() = default;

        explicit GamepadSDL(SDL_Gamepad* in_pSDLGamepad)
        {
            m_pContext = in_pSDLGamepad;

            if (!m_pContext)
                return;

            m_ID = SDL_GetJoystickID(SDL_GetGamepadJoystick(in_pSDLGamepad));
        }

        ~GamepadSDL()
        {
            SDL_CloseGamepad(GetContext<SDL_Gamepad>());
        }

        virtual std::string GetName() const override
        {
            if (const auto result = SDL_GetGamepadName(GetContext<SDL_Gamepad>()))
                return result;

            return Gamepad::GetName();
        }

        virtual PeripheralVendor GetVendor() const override
        {
            PeripheralVendor result{};

            switch (SDL_GetGamepadType(GetContext<SDL_Gamepad>()))
            {
                case SDL_GAMEPAD_TYPE_PS3:
                case SDL_GAMEPAD_TYPE_PS4:
                case SDL_GAMEPAD_TYPE_PS5:
                    result = PeripheralVendor::PlayStation;
                    break;

                default:
                    result = PeripheralVendor::Xbox;
                    break;
            }

            return result;
        }

        virtual XAMINPUT_GAMEPAD GetState() override
        {
            m_State = {};

            const auto pGamepad = GetContext<SDL_Gamepad>();

            m_State.sThumbLX = SDL_GetGamepadAxis(pGamepad, SDL_GAMEPAD_AXIS_LEFTX);
            m_State.sThumbLY = ~SDL_GetGamepadAxis(pGamepad, SDL_GAMEPAD_AXIS_LEFTY);

            m_State.sThumbRX = SDL_GetGamepadAxis(pGamepad, SDL_GAMEPAD_AXIS_RIGHTX);
            m_State.sThumbRY = ~SDL_GetGamepadAxis(pGamepad, SDL_GAMEPAD_AXIS_RIGHTY);

            m_State.bLeftTrigger = SDL_GetGamepadAxis(pGamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) >> 7;
            m_State.bRightTrigger = SDL_GetGamepadAxis(pGamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) >> 7;

            const auto translate = [pGamepad](const auto& in_sdlInput, const auto& in_xamInput) -> int
            {
                return SDL_GetGamepadButton(pGamepad, in_sdlInput) << FirstBitLow(in_xamInput);
            };

            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_DPAD_UP, XAMINPUT_GAMEPAD_DPAD_UP);
            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_DPAD_DOWN, XAMINPUT_GAMEPAD_DPAD_DOWN);
            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_DPAD_LEFT, XAMINPUT_GAMEPAD_DPAD_LEFT);
            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, XAMINPUT_GAMEPAD_DPAD_RIGHT);

            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_START, XAMINPUT_GAMEPAD_START);
            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_BACK, XAMINPUT_GAMEPAD_BACK);
            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_TOUCHPAD, XAMINPUT_GAMEPAD_BACK);

            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_LEFT_STICK, XAMINPUT_GAMEPAD_LEFT_THUMB);
            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_RIGHT_STICK, XAMINPUT_GAMEPAD_RIGHT_THUMB);

            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, XAMINPUT_GAMEPAD_LEFT_SHOULDER);
            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, XAMINPUT_GAMEPAD_RIGHT_SHOULDER);

            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_SOUTH, XAMINPUT_GAMEPAD_A);
            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_EAST, XAMINPUT_GAMEPAD_B);
            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_WEST, XAMINPUT_GAMEPAD_X);
            m_State.wButtons |= translate(SDL_GAMEPAD_BUTTON_NORTH, XAMINPUT_GAMEPAD_Y);

            m_LastState = m_State;

            return m_State;
        }

        virtual bool SetVibration(const XAMINPUT_VIBRATION& in_rVibration, const uint32_t in_duration = 5000) override
        {
            m_LastVibration = in_rVibration;

            return SDL_RumbleGamepad(GetContext<SDL_Gamepad>(), m_LastVibration.wLeftMotorSpeed, m_LastVibration.wRightMotorSpeed, in_duration);
        }

        virtual bool OnProcessMessage(IMessage& io_rMessage) override
        {
            if (io_rMessage.Is<MsgPeripheralAttached>())
            {
                MsgGetPeripheralUsers msgGetPeripheralUsers(this);
                GetManager()->ProcessMessage(msgGetPeripheralUsers);

                const auto pApp = App::s_pApp;

                if (!pApp)
                    return false;

                const auto pGame = App::s_pApp->GetGame();

                if (!pGame)
                    return false;

                for (const auto& rUser : msgGetPeripheralUsers.Users)
                {
                    // In the event that this gamepad is reconnected, this sets the
                    // gamepad LED to the colour associated with the character for
                    // this user.
                    if (const auto pPlayer = pGame->GetPlayer(rUser->ID))
                        ProcessMessage<MsgSetCharacterLED>(pPlayer->m_Name.c_str());
                }
            }
            else if (io_rMessage.Is<MsgSetCharacterLED>())
            {
                ProcessMessage<MsgSetLED>(static_cast<MsgSetCharacterLED*>(&io_rMessage)->Color);
            }
            else if (io_rMessage.Is<MsgSetLED>())
            {
                const auto pMsgSetLED = static_cast<MsgSetLED*>(&io_rMessage);

                const auto a = pMsgSetLED->Color.A() / 255.0f;
                const auto r = uint8_t(pMsgSetLED->Color.R() * a);
                const auto g = uint8_t(pMsgSetLED->Color.G() * a);
                const auto b = uint8_t(pMsgSetLED->Color.B() * a);

                SDL_SetGamepadLED(GetContext<SDL_Gamepad>(), r, g, b);
            }
            else if (io_rMessage.Is<MsgSetVibration>())
            {
                const auto pMsgSetVibration = static_cast<MsgSetVibration*>(&io_rMessage);

                SetVibration(pMsgSetVibration->Vibration, pMsgSetVibration->Duration);
            }

            return Gamepad::OnProcessMessage(io_rMessage);
        }
    };
}
