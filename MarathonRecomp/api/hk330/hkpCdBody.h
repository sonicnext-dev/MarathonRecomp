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
}
