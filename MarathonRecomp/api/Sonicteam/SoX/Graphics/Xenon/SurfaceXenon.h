#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyTexture.h>
#include <Sonicteam/SoX/Graphics/Surface.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class SurfaceXenon : public Surface
    {
    public:
        be<uint32_t> m_Width;
        be<uint32_t> m_Height;
        xpointer<void> m_pD3DDevice;
        xpointer<void> m_pD3DSurface;
        xpointer<MyTexture> m_pTexture;
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_SIZEOF(SurfaceXenon, 0x88);
    MARATHON_ASSERT_OFFSETOF(SurfaceXenon, m_Width, 0x64);
}
