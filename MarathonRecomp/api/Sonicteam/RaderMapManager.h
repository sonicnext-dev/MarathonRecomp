#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class RaderMapManager : public Actor
    {
    public:
        xpointer<void> m_pClump;
        xpointer<void> m_pFrameGP1;
        xpointer<void> m_pFrameGP2;
        MARATHON_INSERT_PADDING(0x0C);
        SoX::Math::Vector m_Transform;
        be<float> m_Zoom;
        MARATHON_INSERT_PADDING(0x0C);
        SoX::Math::Vector m_Field90;
        MARATHON_INSERT_PADDING(0x10);
    };
}
