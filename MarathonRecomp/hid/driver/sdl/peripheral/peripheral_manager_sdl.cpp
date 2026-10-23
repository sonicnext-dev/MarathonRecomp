#include "peripheral_manager_sdl.h"
#include <hid/driver/sdl/peripheral/gamepad_sdl.h>
#include <hid/driver/sdl/peripheral/keyboard_sdl.h>
#include <hid/driver/sdl/peripheral/mouse_sdl.h>
#include <hid/message/messages.h>
#include <hid/peripheral/peripheral_manager.h>
#include <hid/hid.h>
#include <os/logger.h>
#include <SDL3/SDL.h>
#include <ui/game_window.h>
#include <xxHashMap.h>

namespace HID
{
    static PeripheralManagerSDL g_manager{};

    static bool SDL_EventFilter(void*, SDL_Event* in_pEvent)
    {
        switch (in_pEvent->type)
        {
            case SDL_EVENT_GAMEPAD_ADDED:
                g_manager.Attach(std::make_shared<GamepadSDL>(SDL_OpenGamepad(in_pEvent->gdevice.which)));
                break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                g_manager.Detach(g_manager.Find(in_pEvent->gdevice.which));
                break;

            case SDL_EVENT_WINDOW_FOCUS_LOST:
                g_manager.BroadcastMessage<MsgSetVibration>(XAMINPUT_VIBRATION{ 0, 0 });
                break;

            case SDL_EVENT_QUIT:
                g_manager.BroadcastMessage<MsgSetLED>(0, 0, 0);
                break;
        }

        return true;
    }

    bool PeripheralManagerSDL::Init()
    {
        SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_GAMECUBE, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS3, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_PLAYER_LED, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_WII, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_STEAM, "1");
        SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_STEAMDECK, "1");
        SDL_SetHint(SDL_HINT_XINPUT_ENABLED, "1");

        SDL_InitSubSystem(SDL_INIT_EVENTS);
        SDL_AddEventWatch(SDL_EventFilter, nullptr);

        SDL_InitSubSystem(SDL_INIT_GAMEPAD);

        if (const auto mappings = SDL_AddGamepadMappingsFromFile("gamecontrollerdb.txt"); mappings > 0)
            LOGFN("Loaded SDL gamepad mappings: {}", mappings);

        return PeripheralManager::Init();
    }

    bool PeripheralManagerSDL::OnProcessMessage(IMessage& io_rMessage)
    {
        if (io_rMessage.Is<MsgRegisterKeyboard>())
        {
            auto pUser = GetUser(static_cast<MsgRegisterKeyboard*>(&io_rMessage)->UserID);

            if (!pUser)
                pUser = GetUser(0);

            ProcessMessage<MsgUnregisterKeyboard>();

            pUser->Attach(std::move(std::make_shared<KeyboardSDL>()));
            pUser->Attach(std::move(std::make_shared<MouseSDL>()));
        }
        else if (io_rMessage.Is<MsgSetGamepadUser>())
        {
            const auto pMsgSetGamepadUser = static_cast<MsgSetGamepadUser*>(&io_rMessage);
            const auto pPeripheral = GetUser(pMsgSetGamepadUser->UserID)->Find(pMsgSetGamepadUser->PeripheralID);

            if (pPeripheral->GetType() != PeripheralType::Gamepad)
                return false;

            const auto pGamepad = static_cast<GamepadSDL*>(pPeripheral);

            SDL_SetGamepadPlayerIndex(pGamepad->GetContext<SDL_Gamepad>(), pMsgSetGamepadUser->UserID);
        }
        else if (io_rMessage.Is<MsgSetKeyboardUser>())
        {
            const auto userId = static_cast<MsgSetKeyboardUser*>(&io_rMessage)->UserID;

            ProcessMessage<MsgPushGamepads>(userId);
            ProcessMessage<MsgRegisterKeyboard>(userId);
        }

        return PeripheralManager::OnProcessMessage(io_rMessage);
    }

    PeripheralManager* GetManager()
    {
        return &g_manager;
    }
}
