#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/IntersectListener.h>

namespace Sonicteam::SoX::Physics::Havok
{
    class IntersectLineCallback
    {
    public:
        xpointer<void> m_pVftable;
        Math::Vector m_LineStart;
        Math::Vector m_LineEnd;
        bool m_Field30;
        be<float> m_Field34;
        MARATHON_INSERT_PADDING(0x1C);
        be<float> m_Field54;
        MARATHON_INSERT_PADDING(0x08);
        be<uint32_t> m_HitFlags;
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<IntersectListener> m_pIntersectListener;
        Math::Vector m_Field80;
        Math::Vector m_Field90;
    };

    MARATHON_ASSERT_OFFSETOF(IntersectLineCallback, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(IntersectLineCallback, m_LineStart, 0x10);
    MARATHON_ASSERT_OFFSETOF(IntersectLineCallback, m_LineEnd, 0x20);
    MARATHON_ASSERT_OFFSETOF(IntersectLineCallback, m_Field30, 0x30);
    MARATHON_ASSERT_OFFSETOF(IntersectLineCallback, m_Field34, 0x34);
    MARATHON_ASSERT_OFFSETOF(IntersectLineCallback, m_Field54, 0x54);
    MARATHON_ASSERT_OFFSETOF(IntersectLineCallback, m_HitFlags, 0x60);
    MARATHON_ASSERT_OFFSETOF(IntersectLineCallback, m_pIntersectListener, 0x70);
    MARATHON_ASSERT_OFFSETOF(IntersectLineCallback, m_Field80, 0x80);
    MARATHON_ASSERT_OFFSETOF(IntersectLineCallback, m_Field90, 0x90);
    MARATHON_ASSERT_SIZEOF(IntersectLineCallback, 0xA0);
}
