#pragma once

#include <Marathon.inl>
#include <hk330/hkpShape.h>
#include <hk330/hkpSingleShapeContainer.h>

namespace hk330
{
    class hkpBvShape : public hkpShape
    {
    public:
        xpointer<hkpShape> m_childShape;
        xpointer<hkpShape> m_boundingVolumeShape;
    };

    MARATHON_ASSERT_OFFSETOF(hkpBvShape, m_childShape, 0xC);
    MARATHON_ASSERT_OFFSETOF(hkpBvShape, m_boundingVolumeShape, 0x10);
}
