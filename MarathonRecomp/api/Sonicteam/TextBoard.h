#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class TextBoard
    {
    public:
        MARATHON_INSERT_PADDING(0x24);
    };

    MARATHON_ASSERT_SIZEOF(TextBoard, 0x24);
}
