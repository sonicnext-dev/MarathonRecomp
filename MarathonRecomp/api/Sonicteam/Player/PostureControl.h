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
        // Credit to Gordon Ramsay
        enum PostureFlag
        {
            PostureFlag_Ground = 0x1,              // Detects Ground
            PostureFlag_WallBrushing = 0x8,        // Detects brushing against a wall
            PostureFlag_HeadOnWall = 0x10,         // Detects head-on wall collision (will always be enabled w/ WallBrushing)
            PostureFlag_RailGrind = 0x40,          // Rail grinding
            PostureFlag_BeforeFall = 0x100,        // Seems like Neutral or Pre-Fall? The moment before transitioning from the Jump to Fall animation
            PostureFlag_Fall = 0x200,              // FALL
            PostureFlag_WaterCollision = 0x800,    // Water collision (making you slide down an incline)? BeforeFall is often set with this
            PostureFlag_LightDash = 0x4000,        // Light Dashing
            PostureFlag_QuickRotate = 0x8000,      // Is moving the control stick in a non-forward direction (usually only active for a frame since the character instantly rotates)
            PostureFlag_Tentative = 0x10000,       // Is Tentative collision
            PostureFlag_WaterSlide = 0x20000,      // Water sliding
            PostureFlag_Grass = 0x100000,          // Grass
            PostureFlag_DirtClay = 0x200000,       // Dirt/clay
            PostureFlag_Stone = 0x400000,          // Stone
            PostureFlag_Shoreline = 0x1000000      // Shoreline? sand? Uncertain
        };
        // 0x314 ... 0x400
    };
}
