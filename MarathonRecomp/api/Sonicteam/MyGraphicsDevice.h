#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Xenon/DeviceXenon.h>

namespace Sonicteam
{
    class MyGraphicsDevice : public SoX::Graphics::Xenon::DeviceXenon
    {
    public:
        MARATHON_INSERT_PADDING(0x180);
    };
}
