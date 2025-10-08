#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::SoX::Graphics
{
    class Frame;

    class FrameObserver
    {
    public:
        xpointer<void> m_pVftable;
        LinkNode<FrameObserver> m_lnFrameObserver;
        xpointer<Frame> m_pFrameGPStandard;
    };

    MARATHON_ASSERT_OFFSETOF(FrameObserver, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(FrameObserver, m_lnFrameObserver, 0x4);
    MARATHON_ASSERT_OFFSETOF(FrameObserver, m_pFrameGPStandard, 0x10);
    MARATHON_ASSERT_SIZEOF(FrameObserver, 0x14);
}
