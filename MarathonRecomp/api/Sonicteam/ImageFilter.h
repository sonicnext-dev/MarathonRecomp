#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class ImageFilter
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_OFFSETOF(ImageFilter, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(ImageFilter, 8);
}
