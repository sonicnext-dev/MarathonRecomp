#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>

namespace Sonicteam::SoX::Scenery
{
    class TerrainMgr : public IResourceMgr, public System::Singleton<TerrainMgr, 0x82D3C0E0, System::CreateStatic<TerrainMgr, 0x8261AED8>>
    {

    };
}
