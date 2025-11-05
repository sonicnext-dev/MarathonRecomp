#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Scenery/CameraEventCallback.h>
#include <Sonicteam/SoX/Scenery/Sector.h>

namespace Sonicteam::SoX::Scenery
{
    class WorldSector : public Sector, public CameraEventCallback
    {

    };
}
