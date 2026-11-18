#include "message_box.h"
#include <os/logger.h>
#include <SDL3/SDL.h>
#include <ui/game_window.h>
#include <vector>

struct MessageBoxData
{
    SDL_MessageBoxFlags flags;
    const std::string* message;
    const std::vector<SDL_MessageBoxButtonData>* buttons;
    int* choiceOut;
};

static void ShowMessageBox(void* data)
{
    const auto* msgData = static_cast<const MessageBoxData*>(data);

    if (msgData->buttons && !msgData->buttons->empty())
    {
        SDL_MessageBoxData msgBox;
        msgBox.flags = msgData->flags;
        msgBox.window = GameWindow::s_pWindow;
        msgBox.title = GameWindow::GetTitle();
        msgBox.message = msgData->message->c_str();
        msgBox.numbuttons = static_cast<int>(msgData->buttons->size());
        msgBox.buttons = msgData->buttons->data();
        msgBox.colorScheme = nullptr;

        SDL_ShowMessageBox(&msgBox, msgData->choiceOut);
    }
    else
        SDL_ShowSimpleMessageBox(msgData->flags, GameWindow::GetTitle(), msgData->message->c_str(), GameWindow::s_pWindow);
}

void SimpleMessageBox::Show(MessageType type, const std::string& message)
{
    const auto error = type == MessageType::Error;

    // Also log notices delivered to user via message box
    if (error)
        LOGN_ERROR(message);
    else
        LOGN(message);

    MessageBoxData msgData{};
    msgData.flags = error ? SDL_MESSAGEBOX_ERROR : SDL_MESSAGEBOX_INFORMATION;
    msgData.message = &message;
    msgData.buttons = nullptr;
    msgData.choiceOut = nullptr;

    SDL_RunOnMainThread(ShowMessageBox, &msgData, true);
}

void ChoiceMessageBox::Show(MessageType type, const std::string& message, std::span<const std::string> buttons, int* choiceOut)
{
    const auto error = type == MessageType::Error;

    std::vector<SDL_MessageBoxButtonData> sdlButtons;
    sdlButtons.reserve(buttons.size());

    for (int i = buttons.size() - 1; i >= 0; i--)
    {
        sdlButtons.push_back({
            .flags = i == 0 ? SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT : 0,
            .buttonID = i,
            .text = buttons[i].c_str()
        });
    }

    MessageBoxData msgData{};
    msgData.flags = error ? SDL_MESSAGEBOX_ERROR : SDL_MESSAGEBOX_INFORMATION;
    msgData.message = &message;
    msgData.buttons = &sdlButtons;
    msgData.choiceOut = choiceOut;

    SDL_RunOnMainThread(ShowMessageBox, &msgData, true);
}
