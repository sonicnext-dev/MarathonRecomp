#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Device.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Graphics/Surface.h>
#include <d3dxb.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class DeviceXenon : public Device
    {
    public:
        SoX::RefSharedPointer<Graphics::Surface> m_spBackBuffer;
        SoX::RefSharedPointer<Graphics::Surface> m_spDepthStencil;
        MARATHON_INSERT_PADDING(0x80);
        D3DXBSURFACE_PARAMETERS m_SurfaceParamsA;
        D3DXBSURFACE_PARAMETERS m_SurfaceParamsB;
        D3DXBSURFACE_PARAMETERS m_SurfaceParamsC;
        SoX::RefSharedPointer<Graphics::Surface> m_spColorTile2X;
        SoX::RefSharedPointer<Graphics::Surface> m_spDepthTile2X;
        SoX::RefSharedPointer<Graphics::Surface> m_spColorTile4X;
        SoX::RefSharedPointer<Graphics::Surface> m_spDepthTile4X;
    };

    MARATHON_ASSERT_SIZEOF(DeviceXenon, 0x190);
}
