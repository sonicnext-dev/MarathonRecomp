#pragma once

#include <Marathon.inl>
#include <hk330/hkpShape.h>

namespace hk330
{
    class hkpCdBody
    {
    public:
        xpointer<hkpShape> m_shape;
        be<uint32_t> m_shapeKey;
        xpointer<void> m_motion;
        xpointer<hk330::hkpCdBody> m_parent;
    };

    MARATHON_ASSERT_OFFSETOF(hkpCdBody, m_shape, 0x00);
    MARATHON_ASSERT_OFFSETOF(hkpCdBody, m_shapeKey, 0x04);
    MARATHON_ASSERT_OFFSETOF(hkpCdBody, m_motion, 0x08);
    MARATHON_ASSERT_OFFSETOF(hkpCdBody, m_parent, 0x0C);
}
