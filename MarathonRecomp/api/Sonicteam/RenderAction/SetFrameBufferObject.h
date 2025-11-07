#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyRenderProcess.h>

namespace Sonicteam::RenderAction
{
    class SetFrameBufferObject : public MyRenderProcess
    {
    public:
        xpointer<void> m_pFrameBufferObject;
        MARATHON_INSERT_PADDING(0x4);
        bool m_Once;
        bool m_PostProcess;
    };

    MARATHON_ASSERT_SIZEOF(SetFrameBufferObject, 0x44);
    MARATHON_ASSERT_OFFSETOF(SetFrameBufferObject, m_pFrameBufferObject, 0x38);
    MARATHON_ASSERT_OFFSETOF(SetFrameBufferObject, m_Once, 0x40);
    MARATHON_ASSERT_OFFSETOF(SetFrameBufferObject, m_PostProcess, 0x41);
}
