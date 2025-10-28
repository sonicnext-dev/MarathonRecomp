#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>

namespace Sonicteam
{
    class LuaSystemManager : public SoX::IResourceMgr, public System::Singleton<LuaSystemManager, 0x82D35EDC, System::CreateStatic<LuaSystemManager, 0x82162DF8>> {};
}
