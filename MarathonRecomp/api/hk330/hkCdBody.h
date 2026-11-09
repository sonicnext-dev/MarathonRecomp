#pragma once

#include <Marathon.inl>
#include <hk330/hkShape.h>

namespace hk330
{
    class hkCdBody
    {
    public:
        xpointer<hkShape> m_shape;
        be<uint32_t> m_shapeKey;
        xpointer<void> m_motion;
        xpointer<hkCdBody> m_parent;
    };

    MARATHON_ASSERT_OFFSETOF(hkCdBody, m_shape, 0x00);
    MARATHON_ASSERT_OFFSETOF(hkCdBody, m_shapeKey, 0x04);
    MARATHON_ASSERT_OFFSETOF(hkCdBody, m_motion, 0x08);
    MARATHON_ASSERT_OFFSETOF(hkCdBody, m_parent, 0x0C);
}
