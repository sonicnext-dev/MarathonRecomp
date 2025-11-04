#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/VertexBuffer.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class VertexBufferXenon : public VertexBuffer
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x4);
        xpointer<MyGraphicsDevice> m_pGraphicsDevice;
        xpointer<void> m_pD3DVertexDeclaration;
        xpointer<void> m_pD3DVertexBuffer;
        uint32_t m_Flags;
    };

    MARATHON_ASSERT_SIZEOF(VertexBufferXenon, 0x20);
    MARATHON_ASSERT_OFFSETOF(VertexBufferXenon, m_pD3DVertexDeclaration, 0x14);
}
