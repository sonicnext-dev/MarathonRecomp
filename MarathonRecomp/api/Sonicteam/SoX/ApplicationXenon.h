#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Application.h>
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

        MARATHON_INSERT_PADDING(4);
        be<uint64_t> m_FirstTimeBase;
        be<uint64_t> m_FirstPerformanceFrequency;
        be<uint32_t> m_DeviceState;
        xpointer<void> m_pDevice;
        DeviceInfo m_DeviceInfo;
        be<uint64_t> m_DeltaTimePost; // TODO: check if these are in the correct order.
        be<uint64_t> m_DeltaTimePre;  // TODO: check if these are in the correct order.
        bool m_IsInitBuffers;
        MARATHON_INSERT_PADDING(7);
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

    MARATHON_ASSERT_SIZEOF(ApplicationXenon, 0x180);

}
