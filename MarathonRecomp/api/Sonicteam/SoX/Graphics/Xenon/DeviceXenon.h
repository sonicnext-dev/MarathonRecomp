#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Device.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Graphics/Surface.h>
#include <gpu/video.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class DeviceXenon : public Device
    {
    public:
        SoX::RefSharedPointer<Graphics::Surface> m_spBackBuffer;
        SoX::RefSharedPointer<Graphics::Surface> m_spDepthStencil;
        MARATHON_INSERT_PADDING(0x80);
        GuestSurfaceCreateParams m_SurfaceParamA; //if you decido to rename also rename for ApplicationXenon::CreationDevice
        GuestSurfaceCreateParams m_SurfaceParamB;
        GuestSurfaceCreateParams m_SurfaceParamC;
        SoX::RefSharedPointer<Graphics::Surface> m_spColorTile2X;
        SoX::RefSharedPointer<Graphics::Surface> m_spDepthTile2X;
        SoX::RefSharedPointer<Graphics::Surface> m_spColorTile4X;
        SoX::RefSharedPointer<Graphics::Surface> m_spDepthTile4X;
    };

    MARATHON_ASSERT_SIZEOF(DeviceXenon, 0x190);
}
