#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/IndexBuffer.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class IndexBufferXenon : public IndexBuffer
    {
    public:
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        xpointer<void> m_pD3DIndexBuffer;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(IndexBufferXenon, 0x18);
}
