// Stub implementation of MessageWindow for GTA IV
// This file provides minimal stub implementations until GTA IV-specific UI is implemented

#include "message_window.h"

void MessageWindow::Draw()
{
    // TODO: Implement GTA IV message window
}

bool MessageWindow::Open(std::string text, int* result, std::span<std::string> buttons, int defaultButtonIndex, int cancelButtonIndex)
{
    // TODO: Implement GTA IV message window
    // For now, auto-select the default button
    if (result)
        *result = defaultButtonIndex;
    return MSG_CLOSED;
}

void MessageWindow::Close()
{
    s_isVisible = false;
}
