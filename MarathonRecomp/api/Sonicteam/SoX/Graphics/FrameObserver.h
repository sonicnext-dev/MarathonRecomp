#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/FrameGP.h>
#include <Sonicteam/SoX/Graphics/FrameObserver.h>

namespace Sonicteam::SoX::Graphics
{
    class FrameObserver
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(8);
        xpointer<FrameObserver> m_pThis;
        xpointer<FrameGP> m_pFrameGP;
    };
}
