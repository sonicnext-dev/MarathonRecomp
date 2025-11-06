#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Device.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class DeviceXenon : public Device
    {
    public:
        MARATHON_INSERT_PADDING(0x8);
        xpointer<SurfaceXenon> m_pBackBufferSurface;
        xpointer<SurfaceXenon> m_pDepthStencilSurface;
        MARATHON_INSERT_PADDING(0xB4);
    };

    MARATHON_ASSERT_SIZEOF(DeviceXenon, 0x18C);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_pBackBufferSurface, 0xD0);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_pDepthStencilSurface, 0xD4);
}
