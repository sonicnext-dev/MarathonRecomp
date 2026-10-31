#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Application.h>
#include <Sonicteam/SoX/Graphics/Xenon/DeviceXenon.h>
#include <d3dxb.h>

namespace Sonicteam::SoX
{
    class ApplicationXenon : public Engine::Application
    {
    public:
        struct DeviceInfo
        {
            be<uint32_t> DeviceState;
            xpointer<void> pDevice;
            D3DXBSURFACE_PARAMETERS SurfaceParamsA;
            D3DXBSURFACE_PARAMETERS SurfaceParamsB;
            D3DXBSURFACE_PARAMETERS SurfaceParamsC;
            xpointer<void> pColorTile2x;
            xpointer<void> pDepthTile2x;
            xpointer<void> pColorTile4x;
            xpointer<void> pDepthTile4x;
            D3DXBPRESENT_PARAMETERS PresentParameters;
        };

        MARATHON_INSERT_PADDING(0x04);
        be<uint64_t> m_FirstTimeBase;
        be<uint64_t> m_FirstPerformanceFrequency;
        be<uint32_t> m_DeviceState;
        xpointer<void> m_pDevice;
        DeviceInfo m_DeviceInfo;
        be<uint64_t> m_DeltaTimePost; // TODO: check if these are in the correct order.
        be<uint64_t> m_DeltaTimePre;  // TODO: check if these are in the correct order.
        bool m_IsInitBuffers;
        MARATHON_INSERT_PADDING(0x07);
        xpointer<void> m_pFrontBufferTexture;
        xpointer<void> m_pBackBufferSurface;
        xpointer<void> m_pDepthStencilSurface;
        xpointer<void> m_pColorTile2x;
        xpointer<void> m_pDepthTile2x;
        xpointer<void> m_pColorTile4x;
        xpointer<void> m_pDepthTile4x;
        be<uint32_t> m_ShaderVertexCount;
        be<uint32_t> m_PixelShaderVertexCount;
        be<uint32_t> m_hNotification;
        MARATHON_INSERT_PADDING(0x60);
    };

    MARATHON_ASSERT_OFFSETOF(ApplicationXenon::DeviceInfo, DeviceState, 0x00);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon::DeviceInfo, pDevice, 0x04);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon::DeviceInfo, SurfaceParamsA, 0x08);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon::DeviceInfo, SurfaceParamsB, 0x14);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon::DeviceInfo, SurfaceParamsC, 0x20);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon::DeviceInfo, pColorTile2x, 0x2C);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon::DeviceInfo, pDepthTile2x, 0x30);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon::DeviceInfo, pColorTile4x, 0x34);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon::DeviceInfo, pDepthTile4x, 0x38);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon::DeviceInfo, PresentParameters, 0x3C);
    MARATHON_ASSERT_SIZEOF(ApplicationXenon::DeviceInfo, 0xB8);

    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_FirstTimeBase, 0x10);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_FirstPerformanceFrequency, 0x18);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_DeviceState, 0x20);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_pDevice, 0x24);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_DeviceInfo, 0x28);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_DeltaTimePost, 0xE0);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_DeltaTimePre, 0xE8);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_IsInitBuffers, 0xF0);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_pFrontBufferTexture, 0xF8);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_pBackBufferSurface, 0xFC);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_pDepthStencilSurface, 0x100);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_pColorTile2x, 0x104);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_pDepthTile2x, 0x108);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_pColorTile4x, 0x10C);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_pDepthTile4x, 0x110);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_ShaderVertexCount, 0x114);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_PixelShaderVertexCount, 0x118);
    MARATHON_ASSERT_OFFSETOF(ApplicationXenon, m_hNotification, 0x11C);
    MARATHON_ASSERT_SIZEOF(ApplicationXenon, 0x180);
}
