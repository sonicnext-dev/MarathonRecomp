#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Texture.h>
#include <Sonicteam/SoX/Graphics/VertexBuffer.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <d3dxb.h>

namespace Sonicteam::SoX::Graphics
{
    class Device;
}

namespace Sonicteam::SoX::Graphics::Xenon
{
    class VertexBufferXenon : public VertexBuffer
    {
    public:
        xpointer<Device> m_pDevice;
        xpointer<D3DVertexDeclaration> m_pVertexDeclaration; // D3DResource - ?
        xpointer<D3DVertexBuffer> m_pVertexBuffer; // D3DResource - ?
        be<uint32_t> m_SecondaVertexBufferFlag;
    };

    MARATHON_ASSERT_OFFSETOF(VertexBufferXenon, m_pDevice, 0x10);
    MARATHON_ASSERT_OFFSETOF(VertexBufferXenon, m_pVertexDeclaration, 0x14);
    MARATHON_ASSERT_OFFSETOF(VertexBufferXenon, m_pVertexBuffer, 0x18);
    MARATHON_ASSERT_OFFSETOF(VertexBufferXenon, m_SecondaVertexBufferFlag, 0x1C);
    MARATHON_ASSERT_SIZEOF(VertexBufferXenon, 0x20);
}
