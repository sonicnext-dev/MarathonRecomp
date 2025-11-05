#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam
{
    class WorldRenderProcess : public MyRenderProcess
    {
    public:
        xpointer<SoX::Graphics::Xenon::FrameBufferObjectXenon> m_pFBO;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(WorldRenderProcess, 0x40);
}