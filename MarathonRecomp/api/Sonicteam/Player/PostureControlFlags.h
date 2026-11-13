#pragma once

namespace Sonicteam::Player
{
    // TODO: figure out the true source of these flags.
    enum PostureControlFlags : uint32_t
    {
        // The player is grounded.
        PostureControlFlags_Grounded = (1 << 0),

        // The player is brushing against a wall.
        PostureControlFlags_WallSide = (1 << 3),

        // The player is head on against a wall.
        PostureControlFlags_WallFront = (1 << 4),

        // The player is grinding on a rail.
        PostureControlFlags_RailGrind = (1 << 6),

        // The player is in the intermediate state between jumping and falling.
        PostureControlFlags_FallIntermediate = (1 << 8),

        // The player is falling.
        PostureControlFlags_Fall = (1 << 9),

        // The player is on water collision.
        PostureControlFlags_Water = (1 << 11),

        // The player is using a rope.
        PostureControlFlags_Rope = (1 << 12),

        // The player is light dashing.
        PostureControlFlags_LightDash = (1 << 14),

        // The player is rotating in a non-forward direction.
        PostureControlFlags_QuickRotate = (1 << 15),

        // The player is on move-only collision.
        PostureControlFlags_MoveOnly = (1 << 16),

        // The player is water sliding.
        PostureControlFlags_WaterSlide = (1 << 17),

        // The player is on grass collision.
        PostureControlFlags_Grass = (1 << 20),

        // The player is on dirt collision.
        PostureControlFlags_Dirt = (1 << 21),

        // The player is on stone collision.
        PostureControlFlags_Stone = (1 << 22),

        // The player is on sand collision.
        PostureControlFlags_Sand = (1 << 24)
    };
}
