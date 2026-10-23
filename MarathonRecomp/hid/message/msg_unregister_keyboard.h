#pragma once

#include <message.h>

namespace HID
{
    class MsgUnregisterKeyboard : public Message<MsgUnregisterKeyboard> {};
}
