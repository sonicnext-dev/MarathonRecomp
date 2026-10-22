#pragma once

namespace Sonicteam::Player
{
    enum PostureControlFlags : uint32_t
    {
        // The player is grounded.
        PostureControlFlags_Grounded = 0x01,

        // The player is brushing against a wall.
        PostureControlFlags_WallSide = 0x08,

        // The player is head on against a wall.
        PostureControlFlags_WallFront = 0x10,

        // The player is grinding on a rail.
        PostureControlFlags_RailGrind = 0x40,

        // The player is in the intermediate state between jumping and falling.
        PostureControlFlags_FallIntermediate = 0x100,

        // The player is falling.
        PostureControlFlags_Fall = 0x200,

        // The player is on water collision.
        PostureControlFlags_Water = 0x800,

        // The player is light dashing.
        PostureControlFlags_LightDash = 0x4000,

        // The player is rotating in a non-forward direction.
        PostureControlFlags_QuickRotate = 0x8000,

        // The player is on move-only collision.
        PostureControlFlags_MoveOnly = 0x10000,

        // The player is water sliding.
        PostureControlFlags_WaterSlide = 0x20000,

        // The player is on grass collision.
        PostureControlFlags_Grass = 0x100000,

        // The player is on dirt collision.
        PostureControlFlags_Dirt = 0x200000,

        // The player is on stone collision.
        PostureControlFlags_Stone = 0x400000,

        // The player is on sand collision.
        PostureControlFlags_Sand = 0x1000000
    };
}
