#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Xenon/SurfaceXenon.h>
#include <Sonicteam/SoX/RefCountObject.h>

class MyGraphicsDevice;

namespace Sonicteam::SoX::Graphics
{
    class FrameBufferObject: public RefCountObject
    {
    public:
        xpointer<Xenon::SurfaceXenon> m_pRenderTargetSurface;
        MARATHON_INSERT_PADDING(0xC);
        xpointer<Xenon::SurfaceXenon> m_pDepthStencilSurface;
        MARATHON_INSERT_PADDING(0x1C);
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(FrameBufferObject, 0x40);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObject, m_pRenderTargetSurface, 0x8);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObject, m_pDepthStencilSurface, 0x18);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObject, m_pMyGraphicsDevice, 0x38);
}
