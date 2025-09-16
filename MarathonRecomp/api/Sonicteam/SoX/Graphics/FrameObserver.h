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
}
