#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/FrameGP.h>

namespace Sonicteam
{
    class RaderMapManager : public Actor
    {
    public:
        xpointer<void> m_pClump;
        xpointer<SoX::Graphics::FrameGP> m_pFrameGP1;
        xpointer<SoX::Graphics::FrameGP> m_pFrameGP2;
        MARATHON_INSERT_PADDING(0x0C);
        SoX::Math::Vector m_Transform;
        be<float> m_Zoom;
        MARATHON_INSERT_PADDING(0x0C);
        SoX::Math::Vector m_Field90;
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_OFFSETOF(RaderMapManager, m_pClump, 0x58);
    MARATHON_ASSERT_OFFSETOF(RaderMapManager, m_pFrameGP1, 0x5C);
    MARATHON_ASSERT_OFFSETOF(RaderMapManager, m_pFrameGP2, 0x60);
    MARATHON_ASSERT_OFFSETOF(RaderMapManager, m_Transform, 0x70);
    MARATHON_ASSERT_OFFSETOF(RaderMapManager, m_Zoom, 0x80);
    MARATHON_ASSERT_OFFSETOF(RaderMapManager, m_Field90, 0x90);
    MARATHON_ASSERT_SIZEOF(RaderMapManager, 0xB0);
}
