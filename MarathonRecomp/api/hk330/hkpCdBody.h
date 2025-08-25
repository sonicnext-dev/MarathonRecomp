#pragma once

#include <Marathon.inl>
#include <hk330/hkpShape.h>

namespace hk330
{
    class hkpCdBody
    {
    public:
        xpointer<hkpShape> m_shape;
        MARATHON_INSERT_PADDING(0xC);
    };
}

