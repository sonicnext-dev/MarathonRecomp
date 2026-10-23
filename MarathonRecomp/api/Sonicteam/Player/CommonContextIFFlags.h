#pragma once

namespace Sonicteam::Player
{
    enum CommonContextIFFlags : uint32_t
    {
        // The player's inputs are locked.
        CommonContextIFFlags_OutOfControl = 0x100
    };
}
