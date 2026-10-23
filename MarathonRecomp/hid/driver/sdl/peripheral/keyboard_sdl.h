#pragma once

#include <hid/peripheral/keyboard.h>
#include <ui/game_window.h>
#include <user/config.h>

namespace HID
{
    class KeyboardSDL : public Keyboard
    {
    public:
        virtual XAMINPUT_GAMEPAD GetState() override
        {
            m_State = {};

            if (!GameWindow::s_isFocused)
                return m_State;

            const auto pKeyboardState = SDL_GetKeyboardState(nullptr);

            if (pKeyboardState[SDL_SCANCODE_LALT])
                return m_State;

            if (pKeyboardState[Config::Keyboard_LeftStickUp])
                m_State.sThumbLY = 32767;
            if (pKeyboardState[Config::Keyboard_LeftStickDown])
                m_State.sThumbLY = -32768;
            if (pKeyboardState[Config::Keyboard_LeftStickLeft])
                m_State.sThumbLX = -32768;
            if (pKeyboardState[Config::Keyboard_LeftStickRight])
                m_State.sThumbLX = 32767;

            if (pKeyboardState[Config::Keyboard_RightStickUp])
                m_State.sThumbRY = 32767;
            if (pKeyboardState[Config::Keyboard_RightStickDown])
                m_State.sThumbRY = -32768;
            if (pKeyboardState[Config::Keyboard_RightStickLeft])
                m_State.sThumbRX = -32768;
            if (pKeyboardState[Config::Keyboard_RightStickRight])
                m_State.sThumbRX = 32767;

            if (pKeyboardState[Config::Keyboard_LeftTrigger])
                m_State.bLeftTrigger = 0xFF;
            if (pKeyboardState[Config::Keyboard_RightTrigger])
                m_State.bRightTrigger = 0xFF;

            if (pKeyboardState[Config::Keyboard_DPadUp])
                m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_UP;
            if (pKeyboardState[Config::Keyboard_DPadDown])
                m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_DOWN;
            if (pKeyboardState[Config::Keyboard_DPadLeft])
                m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_LEFT;
            if (pKeyboardState[Config::Keyboard_DPadRight])
                m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_RIGHT;

            if (pKeyboardState[Config::Keyboard_Start])
                m_State.wButtons |= XAMINPUT_GAMEPAD_START;
            if (pKeyboardState[Config::Keyboard_Back])
                m_State.wButtons |= XAMINPUT_GAMEPAD_BACK;

            if (pKeyboardState[Config::Keyboard_LeftShoulder])
                m_State.wButtons |= XAMINPUT_GAMEPAD_LEFT_SHOULDER;
            if (pKeyboardState[Config::Keyboard_RightShoulder])
                m_State.wButtons |= XAMINPUT_GAMEPAD_RIGHT_SHOULDER;

            if (pKeyboardState[Config::Keyboard_LeftStick])
                m_State.wButtons |= XAMINPUT_GAMEPAD_LEFT_THUMB;
            if (pKeyboardState[Config::Keyboard_RightStick])
                m_State.wButtons |= XAMINPUT_GAMEPAD_RIGHT_THUMB;

            if (pKeyboardState[Config::Keyboard_South])
                m_State.wButtons |= XAMINPUT_GAMEPAD_A;
            if (pKeyboardState[Config::Keyboard_East])
                m_State.wButtons |= XAMINPUT_GAMEPAD_B;
            if (pKeyboardState[Config::Keyboard_West])
                m_State.wButtons |= XAMINPUT_GAMEPAD_X;
            if (pKeyboardState[Config::Keyboard_North])
                m_State.wButtons |= XAMINPUT_GAMEPAD_Y;

            m_LastState = m_State;

            return m_State;
        }
    };
}
