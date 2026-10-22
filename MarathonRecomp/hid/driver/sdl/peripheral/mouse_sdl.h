#pragma once

#include <hid/message/messages.h>
#include <hid/peripheral/mouse.h>
#include <ui/game_window.h>

namespace HID
{
    class MouseSDL : public Mouse
    {
    public:
        virtual XAMINPUT_GAMEPAD GetState() override
        {
            m_State = {};

            RelativeX = 0;
            RelativeY = 0;

            if (!GameWindow::s_isFocused)
                return m_State;

            const auto processInput = [&](EMouseInput in_input)
            {
                if (in_input == EMouseInput::Unknown)
                    return;

                if (in_input == Config::Mouse_LeftStickUp)
                    m_State.sThumbLY = 32767;
                if (in_input == Config::Mouse_LeftStickDown)
                    m_State.sThumbLY = -32768;
                if (in_input == Config::Mouse_LeftStickLeft)
                    m_State.sThumbLX = -32768;
                if (in_input == Config::Mouse_LeftStickRight)
                    m_State.sThumbLX = 32767;

                if (in_input == Config::Mouse_RightStickUp)
                    m_State.sThumbRY = 32767;
                if (in_input == Config::Mouse_RightStickDown)
                    m_State.sThumbRY = -32768;
                if (in_input == Config::Mouse_RightStickLeft)
                    m_State.sThumbRX = -32768;
                if (in_input == Config::Mouse_RightStickRight)
                    m_State.sThumbRX = 32767;

                if (in_input == Config::Mouse_LeftTrigger)
                    m_State.bLeftTrigger = 0xFF;
                if (in_input == Config::Mouse_RightTrigger)
                    m_State.bRightTrigger = 0xFF;

                if (in_input == Config::Mouse_DPadUp)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_UP;
                if (in_input == Config::Mouse_DPadDown)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_DOWN;
                if (in_input == Config::Mouse_DPadLeft)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_LEFT;
                if (in_input == Config::Mouse_DPadRight)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_RIGHT;

                if (in_input == Config::Mouse_Start)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_START;
                if (in_input == Config::Mouse_Back)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_BACK;

                if (in_input == Config::Mouse_LeftShoulder)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_LEFT_SHOULDER;
                if (in_input == Config::Mouse_RightShoulder)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_RIGHT_SHOULDER;

                if (in_input == Config::Mouse_LeftStick)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_LEFT_THUMB;
                if (in_input == Config::Mouse_RightStick)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_RIGHT_THUMB;

                if (in_input == Config::Mouse_South)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_A;
                if (in_input == Config::Mouse_East)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_B;
                if (in_input == Config::Mouse_West)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_X;
                if (in_input == Config::Mouse_North)
                    m_State.wButtons |= XAMINPUT_GAMEPAD_Y;
            };

            SDL_Event event{};

            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_EVENT_MOUSE_MOTION:
                        X = event.motion.x;
                        Y = event.motion.y;
                        RelativeX += event.motion.xrel;
                        RelativeY += event.motion.yrel;
                        break;

                    case SDL_EVENT_MOUSE_WHEEL:
                    {
                        if (event.wheel.y < 0.0f)
                        {
                            processInput(EMouseInput::WheelDown);
                        }
                        else if (event.wheel.y > 0.0f)
                        {
                            processInput(EMouseInput::WheelUp);
                        }

                        break;
                    }
                }
            }

            const auto buttons = SDL_GetMouseState(nullptr, nullptr);

            if ((buttons & SDL_BUTTON_LMASK) != 0)
                processInput(EMouseInput::LeftClick);
            if ((buttons & SDL_BUTTON_MMASK) != 0)
                processInput(EMouseInput::MiddleClick);
            if ((buttons & SDL_BUTTON_RMASK) != 0)
                processInput(EMouseInput::RightClick);

            if ((buttons & SDL_BUTTON_X1MASK) != 0)
                processInput(EMouseInput::X1);
            if ((buttons & SDL_BUTTON_X2MASK) != 0)
                processInput(EMouseInput::X2);

            m_LastState = m_State;

            return m_State;
        }

        virtual bool OnProcessMessage(IMessage& io_rMessage) override
        {
            if (io_rMessage.Is<MsgPeripheralDetached>())
            {
                SDL_SetWindowRelativeMouseMode(GameWindow::s_pWindow, false);
            }
            else if (io_rMessage.Is<MsgSetMouseCapture>())
            {
                SDL_SetWindowRelativeMouseMode(GameWindow::s_pWindow, static_cast<MsgSetMouseCapture*>(&io_rMessage)->Enabled);
            }

            return Mouse::OnProcessMessage(io_rMessage);
        }
    };
}
