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
            m_State = XAMINPUT_GAMEPAD();

            if (!GameWindow::s_isFocused)
                return m_State;

            float x{}, y{};
            const auto buttons = SDL_GetRelativeMouseState(&x, &y);

            // TODO: mouse look.
            m_State.sThumbRX = uint16_t(x * 32767.0f);
            m_State.sThumbRY = uint16_t(y * 32767.0f);
            
            // TODO: button configuration.
            if ((buttons & SDL_BUTTON_LMASK) != 0)
                m_State.wButtons |= XAMINPUT_GAMEPAD_A;
            if ((buttons & SDL_BUTTON_RMASK) != 0)
                m_State.wButtons |= XAMINPUT_GAMEPAD_X;

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
