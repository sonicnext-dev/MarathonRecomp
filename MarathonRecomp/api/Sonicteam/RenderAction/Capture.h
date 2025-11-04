#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class Capture : public MyRenderProcess
    {
    public:
        MARATHON_INSERT_PADDING(0x4);
        xpointer<SoX::Graphics::Xenon::SurfaceXenon> m_pFramebufferHDR;
        MARATHON_INSERT_PADDING(0x14);
        xpointer<SoX::Graphics::Xenon::FrameBufferObjectXenon> m_pFBO1;
        MARATHON_INSERT_PADDING(0x10);
        xpointer<SoX::Graphics::Xenon::FrameBufferObjectXenon> m_pFBO2;
    };

    MARATHON_ASSERT_SIZEOF(Capture, 0x6C);
    MARATHON_ASSERT_OFFSETOF(Capture, m_pFramebufferHDR, 0x3C);
    MARATHON_ASSERT_OFFSETOF(Capture, m_pFBO1, 0x54);
    MARATHON_ASSERT_OFFSETOF(Capture, m_pFBO2, 0x68);
}
