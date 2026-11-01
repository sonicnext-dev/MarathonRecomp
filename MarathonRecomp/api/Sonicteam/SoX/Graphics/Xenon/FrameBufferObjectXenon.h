#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Xenon/DeviceXenon.h>
#include <Sonicteam/SoX/Graphics/FrameBufferObject.h>
#include <Sonicteam/SoX/Graphics/Texture.h>
#include <stdx/vector.h>
#include <d3dxb.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class FrameBufferObjectXenon : public FrameBufferObject
    {
    public:
        xpointer<DeviceXenon> m_pDevice;
        stdx::vector<D3DXBRECT> m_TileRects;
        be<uint32_t> m_FrameBufferFormat;
        MARATHON_INSERT_PADDING(0x04);
        be<uint32_t> m_DepthStencilFormat;
        MARATHON_INSERT_PADDING(0x18);
        uint8_t m_Field70;
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        bool m_ZFloat;
        bool m_ZPass;
    };

    MARATHON_ASSERT_OFFSETOF(FrameBufferObjectXenon, m_pDevice, 0x38);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObjectXenon, m_TileRects, 0x3C);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObjectXenon, m_FrameBufferFormat, 0x4C);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObjectXenon, m_DepthStencilFormat, 0x54);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObjectXenon, m_Field70, 0x70);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObjectXenon, m_Width, 0x74);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObjectXenon, m_Height, 0x78);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObjectXenon, m_ZFloat, 0x7C);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObjectXenon, m_ZPass, 0x7D);
    MARATHON_ASSERT_SIZEOF(FrameBufferObjectXenon, 0x80);
}
