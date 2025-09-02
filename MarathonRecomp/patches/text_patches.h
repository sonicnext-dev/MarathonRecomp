#pragma once

#include <xxHashMap.h>

class TextPatches
{
public:
    static inline xxHashMap<const char*> s_redirectedMessages
    {
        { HashStr("msg_deviceselect"), "msg_retry" },           // Replace "Select storage device." text with "Retry" for alert windows.
        { HashStr("msg_gamequitconfirm4"), "msg_backtotitle1" } // Replace "Exit the game." text with "Go back to the Title Screen."
    };
};
