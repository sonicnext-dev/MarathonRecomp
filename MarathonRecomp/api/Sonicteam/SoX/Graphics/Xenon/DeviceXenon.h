#pragma once

#include <Marathon.inl>
#include <d3dxb.h>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/SoX/Graphics/Device.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class DeviceXenon : public Device
    {
    public:
        MARATHON_INSERT_PADDING(0x8);
        RefSharedPointer<Surface> m_spBackBuffer;
        RefSharedPointer<Surface> m_spDepthStencil;
        MARATHON_INSERT_PADDING(0x30);
        Array<xpointer<void>, 10> m_apTexture;
        MARATHON_INSERT_PADDING(0x28);
        D3DXBSURFACE_PARAMETERS m_SurfaceParamsA;
        D3DXBSURFACE_PARAMETERS m_SurfaceParamsB;
        D3DXBSURFACE_PARAMETERS m_SurfaceParamsC;
        RefSharedPointer<Surface> m_spColorTile2X;
        RefSharedPointer<Surface> m_spDepthTile2X;
        RefSharedPointer<Surface> m_spColorTile4X;
        RefSharedPointer<Surface> m_spDepthTile4X;
    };

    MARATHON_ASSERT_SIZEOF(DeviceXenon, 0x18C);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_spBackBuffer, 0xD0);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_spDepthStencil, 0xD4);
}
