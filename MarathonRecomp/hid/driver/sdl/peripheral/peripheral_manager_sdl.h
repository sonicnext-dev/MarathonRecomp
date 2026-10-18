#pragma once

#include <hid/peripheral/peripheral_manager.h>

namespace HID
{
    class PeripheralManagerSDL : public PeripheralManager
    {
    public:
        virtual bool Init() override;

        virtual bool OnProcessMessage(IMessage& io_rMessage) override;
    };
}
