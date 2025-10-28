#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>

namespace Sonicteam::SoX::Graphics
{
    class TextureMgr : public SoX::IResourceMgr, public System::Singleton<SurfaceMgr, 0x82D36890, System::CreateStatic<SurfaceMgr, 0x8216D1A0>> {};
}
