#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/IndexBuffer.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <d3dxb.h>

namespace Sonicteam::SoX::Graphics
{
    class Device;
}

namespace Sonicteam::SoX::Graphics::Xenon
{
    class IndexBufferXenon : public IndexBuffer
    {
    public:
        xpointer<Device> m_pDevice;
        xpointer<D3DIndexBuffer> m_pIndexBuffer; // D3DResource - ?
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_OFFSETOF(IndexBufferXenon, m_pDevice, 0xC);
    MARATHON_ASSERT_OFFSETOF(IndexBufferXenon, m_pIndexBuffer, 0x10);
    MARATHON_ASSERT_SIZEOF(IndexBufferXenon, 0x18);
}
