#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Device.h>
#include <Sonicteam/SoX/Graphics/Surface.h>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <d3dxb.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class DeviceXenon : public Device
    {
    public:
        SoX::RefSharedPointer<Graphics::Surface> m_spBackBuffer;
        SoX::RefSharedPointer<Graphics::Surface> m_spDepthStencil;
        MARATHON_INSERT_PADDING(0x30);
        SoX::Array<xpointer<void>, 10> m_apTextures;
        MARATHON_INSERT_PADDING(0x28);
        D3DXBSURFACE_PARAMETERS m_SurfaceParamsA;
        D3DXBSURFACE_PARAMETERS m_SurfaceParamsB;
        D3DXBSURFACE_PARAMETERS m_SurfaceParamsC;
        SoX::RefSharedPointer<Graphics::Surface> m_spColorTile2X;
        SoX::RefSharedPointer<Graphics::Surface> m_spDepthTile2X;
        SoX::RefSharedPointer<Graphics::Surface> m_spColorTile4X;
        SoX::RefSharedPointer<Graphics::Surface> m_spDepthTile4X;
    };

    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_spBackBuffer, 0xD0);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_spDepthStencil, 0xD4);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_apTextures, 0x108);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_SurfaceParamsA, 0x158);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_SurfaceParamsB, 0x164);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_SurfaceParamsC, 0x170);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_spColorTile2X, 0x17C);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_spDepthTile2X, 0x180);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_spColorTile4X, 0x184);
    MARATHON_ASSERT_OFFSETOF(DeviceXenon, m_spDepthTile4X, 0x188);
    MARATHON_ASSERT_SIZEOF(DeviceXenon, 0x18C);
}
