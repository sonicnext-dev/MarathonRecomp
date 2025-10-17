#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPostureControl.h>
#include <Sonicteam/Player/IPostureSupportSphere.h>
#include <Sonicteam/Player/IPostureSupportOttoto.h>
#include <Sonicteam/Player/IPostureSupportEdge.h>
#include <Sonicteam/Player/IPostureSupportInput.h>
#include <Sonicteam/Player/IPostureSupportRayTemplate.h>
#include <Sonicteam/Player/GroundRayListener.h>

namespace Sonicteam::Player
{
    class PostureControl : public IPostureControl, public IPostureSupportSphere, public IPostureSupportOttoto, public IPostureSupportEdge, public IPostureSupportInput, public IPostureSupportRayTemplate<GroundRayListener>
    {
    public:
        enum PostureFlag
        {
            // The player is grounded.
            PostureFlag_Grounded = 0x01,

            // The player is brushing against a wall.
            PostureFlag_WallSide = 0x08,

            // The player is head on against a wall.
            PostureFlag_WallFront = 0x10,

            // The player is grinding on a rail.
            PostureFlag_RailGrind = 0x40,

            // The player is in the intermediate state between jumping and falling.
            PostureFlag_FallIntermediate = 0x100,

            // The player is falling.
            PostureFlag_Fall = 0x200,

            // The player is on water collision.
            PostureFlag_Water = 0x800,

            // The player is light dashing.
            PostureFlag_LightDash = 0x4000,

            // The player is rotating in a non-forward direction.
            PostureFlag_QuickRotate = 0x8000,

            // The player is on tentative collision.
            PostureFlag_Tentative = 0x10000,

            // The player is water sliding.
            PostureFlag_WaterSlide = 0x20000,

            // The player is on grass collision.
            PostureFlag_Grass = 0x100000,

            // The player is on dirt collision.
            PostureFlag_Dirt = 0x200000,

            // The player is on stone collision.
            PostureFlag_Stone = 0x400000,

            // The player is on sand collision.
            PostureFlag_Sand = 0x1000000
        };
    };
}
