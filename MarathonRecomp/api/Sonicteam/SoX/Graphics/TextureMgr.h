#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/System/CreateStatic.h>

namespace Sonicteam::SoX::Graphics
{
    class TextureMgr : public IResourceMgr, public System::Singleton<TextureMgr, 0x82D36890, System::CreateStatic<TextureMgr, 0x8216D1A0>>
    {

    };
}
