#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/FrameGP.h>

namespace Sonicteam::SoX::Graphics
{
    class FrameObserver
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(8);
        xpointer<FrameObserver> m_pParent;
        xpointer<FrameGP> m_pFrameGP;
    };

    MARATHON_ASSERT_OFFSETOF(FrameObserver, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(FrameObserver, m_pParent, 0x0C);
    MARATHON_ASSERT_OFFSETOF(FrameObserver, m_pFrameGP, 0x10);
    MARATHON_ASSERT_SIZEOF(FrameObserver, 0x14);
}
