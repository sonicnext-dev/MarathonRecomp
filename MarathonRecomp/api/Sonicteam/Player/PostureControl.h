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
        //Gordon Ramsay
        enum ePostureFlag
        {
            ePostureFlag_Ground = 0x1,              // detects Ground 
            ePostureFlag_WallBrushing = 0x8,        // detects brushing against a wall
            ePostureFlag_HeadOnWall = 0x10,         // detects head-on wall collision (will always be enabled w/ WallBrushing)
            ePostureFlag_RailGrind = 0x40,          // Rail grinding
            ePostureFlag_BeforeFall = 0x100,        // seems like Neutral or Pre-Fall? The moment before transitioning from the Jump to Fall animation
            ePostureFlag_Fall = 0x200,              // FALL
            ePostureFlag_WaterCollision = 0x800,    // Water collision (making you slide down an incline)? BeforeFall is often set with this
            ePostureFlag_LightDash = 0x4000,        // Light Dashing
            ePostureFlag_QuickRotate = 0x8000,      // is moving the control stick in a non-forward direction (usually only active for a frame since the character instantly rotates)
            ePostureFlag_Tentative = 0x10000,       // is Tentative collision 
            ePostureFlag_WaterSlide = 0x20000,      // Water sliding
            ePostureFlag_Grass = 0x100000,          // grass
            ePostureFlag_DirtClay = 0x200000,       // dirt/clay
            ePostureFlag_Stone = 0x400000,          // stone
            ePostureFlag_Shoreline = 0x1000000      // shoreline? sand? Uncertain
        };
    };
}
