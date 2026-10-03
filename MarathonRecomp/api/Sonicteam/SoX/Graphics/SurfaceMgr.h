#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>

namespace Sonicteam::SoX::Graphics
{
    class SurfaceMgr : public SoX::IResourceMgr, public System::Singleton<SurfaceMgr, 0x82D36888, System::CreateStatic<SurfaceMgr, 0x8216D098>> {};
}
