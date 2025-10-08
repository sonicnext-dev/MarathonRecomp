#pragma once

#include <Marathon.inl>
#include <Sonicteam/PathAnimation/Animation.h>

namespace Sonicteam
{
    class KeyframedPath : public PathAnimation::Animation
    {
        MARATHON_INSERT_PADDING(0x14);
    };

    MARATHON_ASSERT_SIZEOF(KeyframedPath, 0x1C);
}
