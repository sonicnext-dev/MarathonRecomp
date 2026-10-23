#pragma once

#include <message.h>

namespace HID
{
    class MsgSetMouseCapture : public Message<MsgSetMouseCapture>
    {
    public:
        bool Enabled{};

        MsgSetMouseCapture(bool in_enabled) : Enabled(in_enabled) {}
    };
}
