#pragma once

#include <Marathon.inl>
#include <d3dxb.h>

namespace Sonicteam::SoX::Graphics
{
    class Device;

    class ShaderPassFXL
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<Device> m_pDevice;
        xpointer<IDirect3DVertexShader9> m_VertexShader;
        xpointer<IDirect3DPixelShader9> m_PixelShader;
    };

    MARATHON_ASSERT_SIZEOF(ShaderPassFXL, 0x10);
}
