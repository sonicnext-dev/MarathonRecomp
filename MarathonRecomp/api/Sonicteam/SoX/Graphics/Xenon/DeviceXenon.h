#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Device.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class DeviceXenon : public Device
    {
    public:
        MARATHON_INSERT_PADDING(0xC0);
    };

    MARATHON_ASSERT_SIZEOF(DeviceXenon, 0x190);
}
