#pragma once

#include <SDL.h>
#include <ui/game_window.h>

#define SDL_USER_PLAYER_CHAR (SDL_USEREVENT + 1)

inline void SDL_ResizeEvent(SDL_Window* pWindow, int width, int height)
{
    SDL_Event event{};
    event.type = SDL_WINDOWEVENT;
    event.window.event = SDL_WINDOWEVENT_RESIZED;
    event.window.windowID = SDL_GetWindowID(pWindow);
    event.window.data1 = width;
    event.window.data2 = height;

    SDL_PushEvent(&event);
}

inline void SDL_MoveEvent(SDL_Window* pWindow, int x, int y)
{
    SDL_Event event{};
    event.type = SDL_WINDOWEVENT;
    event.window.event = SDL_WINDOWEVENT_MOVED;
    event.window.windowID = SDL_GetWindowID(pWindow);
    event.window.data1 = x;
    event.window.data2 = y;

    SDL_PushEvent(&event);
}

inline void SDL_User_PlayerChar(EPlayerCharacter character)
{
    SDL_Event event{};
    event.type = SDL_USER_PLAYER_CHAR;
    event.user.code = static_cast<Sint32>(character);

    SDL_PushEvent(&event);
}
