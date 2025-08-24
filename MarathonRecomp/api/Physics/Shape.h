#pragma once

#include <Marathon.inl>

namespace Phys
{
    class Shape
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x34);
        be<uint32_t> m_CollisionFilter;
        MARATHON_INSERT_PADDING(0x34);
    };
}

