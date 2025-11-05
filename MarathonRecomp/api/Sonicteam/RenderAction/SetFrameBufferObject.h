#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetFrameBufferObject : public MyRenderProcess
    {
    public:
        xpointer<SoX::Graphics::Xenon::FrameBufferObjectXenon> m_pFrameBufferObject;
        MARATHON_INSERT_PADDING(0x8);
    };

    MARATHON_ASSERT_SIZEOF(SetFrameBufferObject, 0x44);
    MARATHON_ASSERT_OFFSETOF(SetFrameBufferObject, m_pParentProcess, 0x30);
}
