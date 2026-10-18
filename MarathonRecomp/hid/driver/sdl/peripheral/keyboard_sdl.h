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
            m_State = XAMINPUT_GAMEPAD();

            if (!GameWindow::s_isFocused)
                return m_State;

            const auto pKeyboardState = SDL_GetKeyboardState(nullptr);

            if (pKeyboardState[SDL_SCANCODE_LALT])
                return m_State;

            if (pKeyboardState[Config::Key_LeftStickUp])
                m_State.sThumbLY = 32767;
            if (pKeyboardState[Config::Key_LeftStickDown])
                m_State.sThumbLY = -32768;
            if (pKeyboardState[Config::Key_LeftStickLeft])
                m_State.sThumbLX = -32768;
            if (pKeyboardState[Config::Key_LeftStickRight])
                m_State.sThumbLX = 32767;

            if (pKeyboardState[Config::Key_RightStickUp])
                m_State.sThumbRY = 32767;
            if (pKeyboardState[Config::Key_RightStickDown])
                m_State.sThumbRY = -32768;
            if (pKeyboardState[Config::Key_RightStickLeft])
                m_State.sThumbRX = -32768;
            if (pKeyboardState[Config::Key_RightStickRight])
                m_State.sThumbRX = 32767;

            if (pKeyboardState[Config::Key_LeftTrigger])
                m_State.bLeftTrigger = 0xFF;
            if (pKeyboardState[Config::Key_RightTrigger])
                m_State.bRightTrigger = 0xFF;

            if (pKeyboardState[Config::Key_DPadUp])
                m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_UP;
            if (pKeyboardState[Config::Key_DPadDown])
                m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_DOWN;
            if (pKeyboardState[Config::Key_DPadLeft])
                m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_LEFT;
            if (pKeyboardState[Config::Key_DPadRight])
                m_State.wButtons |= XAMINPUT_GAMEPAD_DPAD_RIGHT;

            if (pKeyboardState[Config::Key_Start])
                m_State.wButtons |= XAMINPUT_GAMEPAD_START;
            if (pKeyboardState[Config::Key_Back])
                m_State.wButtons |= XAMINPUT_GAMEPAD_BACK;

            if (pKeyboardState[Config::Key_LeftShoulder])
                m_State.wButtons |= XAMINPUT_GAMEPAD_LEFT_SHOULDER;
            if (pKeyboardState[Config::Key_RightShoulder])
                m_State.wButtons |= XAMINPUT_GAMEPAD_RIGHT_SHOULDER;

            if (pKeyboardState[Config::Key_LeftStick])
                m_State.wButtons |= XAMINPUT_GAMEPAD_LEFT_THUMB;
            if (pKeyboardState[Config::Key_RightStick])
                m_State.wButtons |= XAMINPUT_GAMEPAD_RIGHT_THUMB;

            if (pKeyboardState[Config::Key_South])
                m_State.wButtons |= XAMINPUT_GAMEPAD_A;
            if (pKeyboardState[Config::Key_East])
                m_State.wButtons |= XAMINPUT_GAMEPAD_B;
            if (pKeyboardState[Config::Key_West])
                m_State.wButtons |= XAMINPUT_GAMEPAD_X;
            if (pKeyboardState[Config::Key_North])
                m_State.wButtons |= XAMINPUT_GAMEPAD_Y;

            m_LastState = m_State;

            return m_State;
        }
    };
}
